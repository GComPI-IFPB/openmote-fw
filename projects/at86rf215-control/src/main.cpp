/**
 * @file       main.cpp
 * @author     Pere Tuset-Peiro (peretuset@openmote.com)
 * @version    v0.1
 * @date       January, 2019
 * @brief
 *
 * @copyright  Copyright 2019, OpenMote Technologies, S.L.
 *             This file is licensed under the GNU General Public License v2.
 */

/*================================ include ==================================*/

#include "BoardImplementation.hpp"

#include "Gpio.hpp"

#include "Callback.hpp"
#include "Scheduler.hpp"
#include "Semaphore.hpp"
#include "Task.hpp"

#include "Serial.hpp"
#include "SerialParser.hpp"

#include "At86rf215.hpp"
#include "At86rf215_conf.h"

/*================================ define ===================================*/

#define GREEN_LED_TASK_PRIORITY             ( tskIDLE_PRIORITY + 0 )
#define SERIAL_PARSER_TASK_PRIORITY         ( tskIDLE_PRIORITY + 1 )

#define UART_BAUDRATE						            ( 1267200 )
#define SPI_BAUDRATE                        ( 16000000 )

#define RADIO_RX_BUFFER_LENGTH              ( 128 )
#define RADIO_TX_BUFFER_LENGTH              ( 123 )
#define RADIO_TX_BUFFER_FILL                ( 0x55 )

/*================================ typedef ==================================*/

/*=============================== prototypes ================================*/

static void prvGreenLedTask(void *pvParameters);

static bool radio_on(uint8_t rc);
static bool radio_off(uint8_t rc);
static bool radio_reset(uint8_t rc);
static bool radio_config(uint8_t rc, uint8_t settings, uint8_t frequency, uint8_t length, uint8_t power);
static bool radio_receive_packet(uint8_t rc, uint8_t timeout_ms);
static bool radio_transmit_packet(uint8_t rc);
static bool radio_transmit_continuous(uint8_t rc, bool enable);
static At86rf215::RadioCore get_radio_core(uint8_t rc);

static void radio_rx_init(void);
static void radio_rx_done(void);
static void radio_tx_init(void);
static void radio_tx_done(void);

/*=============================== variables =================================*/

static Serial serial(uart0);

static SerialCallbacks serialCallbacks
{
  .on       = radio_on,
  .off      = radio_off,
  .reset    = radio_reset,
  .config   = radio_config,
  .receive  = radio_receive_packet,
  .transmit = radio_transmit_packet,
  .transmit_continuous = radio_transmit_continuous
};

static Task heartbeatTask{(const char *) "Green", 128, GREEN_LED_TASK_PRIORITY, prvGreenLedTask, nullptr};
static SerialParser serialParser{(const char *) "SerialParser", 128, SERIAL_PARSER_TASK_PRIORITY, serial, serialCallbacks};

static PlainCallback radio_rx_init_cb(&radio_rx_init);
static PlainCallback radio_rx_done_cb(&radio_rx_done);
static PlainCallback radio_tx_init_cb(&radio_tx_init);
static PlainCallback radio_tx_done_cb(&radio_tx_done);

static SemaphoreBinary semaphore_rx(false);
static SemaphoreBinary semaphore_tx(false);

static uint8_t radio_rx_buffer[RADIO_RX_BUFFER_LENGTH];
static uint16_t radio_rx_buffer_len = sizeof(radio_rx_buffer);

static uint8_t radio_tx_buffer[RADIO_TX_BUFFER_LENGTH];
static uint16_t radio_tx_buffer_len = sizeof(radio_tx_buffer);

/*================================= public ==================================*/

int main(void)
{
  /* Initialize the board */
  board.init();
  
  /* Enable the SPI interface */
  spi0.enable(SPI_BAUDRATE);
  
  /* Enable the UART interface */
  uart0.enable(UART_BAUDRATE);
  
  /* Initialize Serial interface */
  serial.init();

  /* Start the scheduler */
  Scheduler::run();
}

/*================================ private ==================================*/

static void prvGreenLedTask(void *pvParameters)
{
  /* Forever */
  while (true)
  {
    /* Turn on green LED for 100 ms */
    led_green.on();
    Scheduler::delay_ms(100);
    
    /* Turn off green LED for 900 ms */
    led_green.off();
    Scheduler::delay_ms(900);
  }
}

static At86rf215::RadioCore get_radio_core(uint8_t val)
{
  At86rf215::RadioCore rc;
  
  /* Set radio core */
  if (val == At86rf215::CORE_RF09 ||
      val == At86rf215::CORE_RF24)
  {
    rc = (At86rf215::RadioCore)val;
  }
  else
  {
    board.error();
  }
  
  return rc;
}

static bool radio_on(uint8_t rc)
{
  At86rf215::RadioCore rc_;
  bool status;
  
  /* Turn on orange LED */
  led_orange.on();
  
  /* Get radio core */
  rc_ = get_radio_core(rc);
  
  /* Turn AT86RF215 radio on */
  at86rf215.on();
  
  /* Check AT86RF215 radio */
  status = at86rf215.check();
  if (!status)
  {
     return false;
  }
  
  /* Set radio callbacks and enable interrupts */
  at86rf215.setRxCallbacks(rc_, &radio_rx_init_cb, &radio_rx_done_cb);
  at86rf215.setTxCallbacks(rc_, &radio_tx_init_cb, &radio_tx_done_cb);
  at86rf215.enableInterrupts();
  
  /* Prepare radio_tx_buffer */
  for (uint16_t i = 0; i < radio_tx_buffer_len; i++)
  {
    radio_tx_buffer[i] = RADIO_TX_BUFFER_FILL;
  }
  
  return true;
}

static bool radio_off(uint8_t rc)
{
  At86rf215::RadioCore rc_;
  
  /* Get radio core */
  rc_ = get_radio_core(rc);
  
  /* Turn AT86RF215 radio off */
  at86rf215.off();
  
  /* Turn off orange LED */
  led_orange.off();
  
  return true;
}

static bool radio_reset(uint8_t rc)
{
  At86rf215::RadioCore rc_;
  
  /* Get radio core */
  rc_ = get_radio_core(rc);
  
  /* Reset radio */
  at86rf215.hardReset();
  
  /* Turn off yellow and orange LEDs */
  led_yellow.off();
  led_orange.off();
  
  board.reset();
  
  return true;
}

static bool radio_config(uint8_t rc, uint8_t settings, uint8_t frequency, uint8_t length, uint8_t power)
{
  At86rf215::RadioCore rc_;
  
  /* Get radio core */
  rc_ = get_radio_core(rc);
  
  /* Wake up and configure radio */
  at86rf215.wakeup(rc_);
  
  /* Set radio configuration and frequency */
  if ((settings <= CONFIG_ELEMENTS) &&
      (frequency <= FREQUENCY_ELEMENTS))
  {
    at86rf215.configure(rc_, &radio_settings[settings], &frequency_settings[frequency]);
  }
  else
  {
    return false;
  }
  
  /* Set transmit power */
  if (power <= At86rf215::TX_POWER_MAX)
  {
    at86rf215.setTransmitPower(rc_, (At86rf215::TransmitPower)power);
  }
  else
  {
    return false;
  }
  
  /* Set packet length */
  radio_tx_buffer_len = length;
  
  return true;
}

static bool radio_receive_packet(uint8_t rc, uint8_t timeout_ms)
{
  At86rf215::RadioCore rc_;
  At86rf215::RadioResult result;
  int8_t rssi, lqi;
  bool received;
  bool crc;
  
  /* Get radio core */
  rc_ = get_radio_core(rc);
  
  /* Ready to transmit */
  at86rf215.ready(rc_);
  
  /* Try to receive a packet */
  at86rf215.receive(rc_);
  
  /* Wait until packet has been received */
  received = semaphore_rx.take(10 * timeout_ms);
  
  /* If we have received a packet */
  if (received == true)
  {
    /* Load packet to radio */
    result = at86rf215.getPacket(rc_, radio_rx_buffer, &radio_rx_buffer_len, &rssi, &lqi, &crc);
    
    /* Check packet has been received successfully */
    if (result == At86rf215::Success && crc == true)
    {
      received = true;
    }
    else
    {
      received = false;
    }
  }
  else
  {
    received = false;
  }
  
  /* Turn off orange LED */
  led_orange.off();
  
  return received;
}

static bool radio_transmit_packet(uint8_t rc)
{
  At86rf215::RadioCore rc_;
  bool transmitted;
  
  /* Get radio core */
  rc_ = get_radio_core(rc);
  
  /* Ready to transmit */
  at86rf215.ready(rc_);
  
  /* Load packet to radio */
  at86rf215.loadPacket(rc_, radio_tx_buffer, radio_tx_buffer_len);
  
  /* Transmit packet */
  at86rf215.transmit(rc_);
  
  /* Wait until packet has been transmitted */
  transmitted = semaphore_tx.take();
  
  return transmitted;
}

static bool radio_transmit_continuous(uint8_t rc, bool enable)
{
  At86rf215::RadioCore rc_;
  
  /* Get radio core */
  rc_ = get_radio_core(rc);
  
  /* Set radio to normal transmit mode */
  at86rf215.setContinuousTransmission(rc_, enable);
  
  /* Ready to transmit */
  at86rf215.ready(rc_);
  
  /* Load packet to radio */
  at86rf215.loadPacket(rc_, radio_tx_buffer, radio_tx_buffer_len);
  
  /* Transmit packet */
  at86rf215.transmit(rc_);
  
  return true;
}

static void radio_rx_init(void)
{
  /* Turn on orange LED */
  led_yellow.on();
}

static void radio_rx_done(void)
{
  /* Turn off orange LED */
  led_yellow.off();

  /* Notify we have transmitted a packet */
  semaphore_rx.giveFromInterrupt();
}

static void radio_tx_init(void)
{
  /* Turn on orange LED */
  led_yellow.on();
}

static void radio_tx_done(void)
{
  /* Turn off orange LED */
  led_yellow.off();

  /* Notify we have transmitted a packet */
  semaphore_tx.giveFromInterrupt();
}
