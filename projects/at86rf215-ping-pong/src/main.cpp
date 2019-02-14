/**
 * @file       main.cpp
 * @author     Pere Tuset-Peiro (peretuset@openmote.com)
 * @version    v0.1
 * @date       February, 2019
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

#include "At86rf215.hpp"
#include "At86rf215_conf.h"

/*================================ define ===================================*/

#define GREEN_LED_TASK_PRIORITY             ( tskIDLE_PRIORITY + 0 )
#define RADIO_TASK_PRIORITY                 ( tskIDLE_PRIORITY + 1 )
#define SERIAL_TASK_PRIORITY                ( tskIDLE_PRIORITY + 2 )

#define UART_BAUDRATE                       ( 921600 )
#define SPI_BAUDRATE                        ( 16000000 )

#define BUFFER_LENGTH                       ( 1024 )

/*================================ typedef ==================================*/

/*=============================== prototypes ================================*/

static void prvGreenLedTask(void *pvParameters);
static void prvRadioTask(void *pvParameters);
static void prvSerialTask(void *pvParameters);

static void radio_rx_init(void);
static void radio_rx_done(void);
static void radio_tx_init(void);
static void radio_tx_done(void);

/*=============================== variables =================================*/

static Serial serial(uart);

static Task heartbeatTask{(const char *) "Green", 128, GREEN_LED_TASK_PRIORITY, prvGreenLedTask, nullptr};
static Task radioTask{(const char *) "Radio", 128, RADIO_TASK_PRIORITY, prvRadioTask, nullptr};
static Task serialTask{(const char *) "Serial", 128, SERIAL_TASK_PRIORITY, prvSerialTask, nullptr};

static PlainCallback radio_rx_init_cb(&radio_rx_init);
static PlainCallback radio_rx_done_cb(&radio_rx_done);
static PlainCallback radio_tx_init_cb(&radio_tx_init);
static PlainCallback radio_tx_done_cb(&radio_tx_done);

static SemaphoreBinary tx_semaphore(false);
static SemaphoreBinary rx_semaphore(false);

static uint8_t serial_rx_buffer[BUFFER_LENGTH];
static uint16_t serial_rx_buffer_len = sizeof(serial_rx_buffer);

static uint8_t radio_rx_buffer[BUFFER_LENGTH];
static uint16_t radio_rx_buffer_len = sizeof(radio_rx_buffer);

static uint8_t radio_tx_buffer[BUFFER_LENGTH];
static uint16_t radio_tx_buffer_len = sizeof(radio_tx_buffer);

static bool received = false;
static bool transmit = false;

/*================================= public ==================================*/

int main(void)
{
  /* Initialize the board */
  board.init();
  
  /* Enable the SPI interface */
  spi.enable(SPI_BAUDRATE);
  
  /* Enable the UART interface */
  uart.enable(UART_BAUDRATE);
  
  /* Initialize Serial interface */
  serial.init();

  /* Start the scheduler */
  Scheduler::run();
}

/*================================ private ==================================*/

static void prvRadioTask(void *pvParameters)
{
  bool status; 

  /* Turn AT86RF215 radio on */
  at86rf215.on();
  
  /* Check AT86RF215 radio */
  status = at86rf215.check();
  if (!status)
  {
    /* Blink red LED */ 
    board.error();
  }
  
  /* Set radio callbacks and enable interrupts */
  at86rf215.setTxCallbacks(At86rf215::CORE_RF09, &radio_tx_init_cb, &radio_tx_done_cb);
  at86rf215.setRxCallbacks(At86rf215::CORE_RF09, &radio_rx_init_cb, &radio_rx_done_cb);
  at86rf215.enableInterrupts();
  
  /* Wake up and configure radio */
  at86rf215.wakeup(At86rf215::CORE_RF09);
  at86rf215.configure(At86rf215::CORE_RF09,
                      &radio_settings[CONFIG_OFDM_2_MCS_5],
                      &frequency_settings[FREQUENCY_OFDM_2]);
  at86rf215.setTransmitPower(At86rf215::CORE_RF09, At86rf215::TransmitPower::TX_POWER_31);

  /* Ready to transmit */
  at86rf215.ready(At86rf215::CORE_RF09);
  
  /* Forever */
  while (true)
  {
    At86rf215::RadioResult result;

    /* We don't have anything to trasnmit, so receive */
    if (!transmit)
    {
      int8_t rssi, lqi;
      bool taken;
      bool crc;

      /* Initialize packet pointer and length */
      uint8_t* buffer_ptr = radio_rx_buffer;
      uint16_t buffer_len = radio_rx_buffer_len;

      /* Transmit packet */
      at86rf215.receive(At86rf215::CORE_RF09);
      
      /* Wait until packet has been received */
      taken = rx_semaphore.take();
      
      /* If we have received a packet */
      if (taken == true && received == true)
      {
        /* Load packet to radio */
        result = at86rf215.getPacket(At86rf215::CORE_RF09, buffer_ptr, &buffer_len, &rssi, &lqi, &crc);
        
        /* Check packet has been received successfully */
        if (result == RadioResult::RadioResult_Success && crc == true)
        {
          /* Write radio buffer via Serial */
          serial.write(buffer_ptr, buffer_len, true);
        }
      }
      else
      {
        led_yellow.off();
      }
      
      /* Go back to ready */
      at86rf215.ready(At86rf215::CORE_RF09);
    }
    /* We got a packet to transmit, fire it! */
    else
    {
      bool sent;
      
      /* Initialize packet pointer and length */
      uint8_t* buffer_ptr = radio_tx_buffer;
      uint16_t buffer_len = radio_tx_buffer_len;

      /* Load packet to radio */
      at86rf215.loadPacket(At86rf215::CORE_RF09, buffer_ptr, buffer_len);
      
      /* Transmit packet */
      at86rf215.transmit(At86rf215::CORE_RF09);
      
      /* Wait until packet has been transmitted */
      sent = tx_semaphore.take();
      if (sent == true)
      {
        led_red.on();
        Scheduler::delay_ms(1);
        led_red.off();
      }
      
      /* Go back to ready */
      at86rf215.ready(At86rf215::CORE_RF09);

      transmit = false;
    }
  }
  
  /* Turn AT86RF215 radio off */
  at86rf215.off();
}

static void prvSerialTask(void *pvParameters)
{
  /* Forever */
  while (true)
  {
    /* Restore serial_rx pointer and length */
    uint8_t* serial_rx_ptr = serial_rx_buffer;
    uint16_t serial_rx_len = sizeof(serial_rx_buffer);

    /* Read buffer using Serial */
    serial_rx_len = serial.read(serial_rx_ptr, serial_rx_len);
    
    /* If we have received a message */
    if (serial_rx_len > 0)
    {
      /* Copy packet to radio transmit buffer */
      dma.memcpy(radio_tx_buffer, serial_rx_ptr, serial_rx_len);
      radio_tx_buffer_len = serial_rx_len;
        
      /* Mark that we have not received, but we need to transmit */
      received = false;
      transmit = true;
      
      /* Notify radio to transmit */
      rx_semaphore.give();
    }
  }
}

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

static void radio_tx_init(void)
{
  /* Turn on orange LED */
  led_orange.on();
}

static void radio_tx_done(void)
{
  /* Turn off orange LED */
  led_orange.off();

  /* Notify we have transmitted a packet */
  tx_semaphore.giveFromInterrupt();
}

static void radio_rx_init(void)
{
  /* Turn on yellow LED */
  led_yellow.on();
}

static void radio_rx_done(void)
{
  /* Turn off yellow LED */
  led_yellow.off();
  
  /* Mark that we have received a packet */
  received = true;

  /* Notify we have received a packet */
  rx_semaphore.giveFromInterrupt();
}