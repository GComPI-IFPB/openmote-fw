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

#include "At86rf215.hpp"
#include "At86rf215_conf.h"

/*================================ define ===================================*/

#define GREEN_LED_TASK_PRIORITY             ( tskIDLE_PRIORITY + 0 )
#define RADIO_TASK_PRIORITY                 ( tskIDLE_PRIORITY + 1 )

#define UART_BAUDRATE						            ( 921600 )
#define SPI_BAUDRATE                        ( 16000000 )

#define RADIO_CORE                          ( At86rf215::CORE_RF09 )
#define RADIO_SETTINGS                      ( &radio_settings[CONFIG_OFDM_2_MCS_5] )
#define RADIO_FREQUENCY                     ( &frequency_settings[FREQUENCY_OFDM_2] )
#define RADIO_TX_POWER                      ( At86rf215::TransmitPower::TX_POWER_MIN )

/*================================ typedef ==================================*/

/*=============================== prototypes ================================*/

static void prvGreenLedTask(void *pvParameters);
static void prvRadioTask(void *pvParameters);

static void radio_tx_init(void);
static void radio_tx_done(void);

/*=============================== variables =================================*/

static Serial serial(uart);

static Task heartbeatTask{(const char *) "Green", 128, GREEN_LED_TASK_PRIORITY, prvGreenLedTask, nullptr};
static Task radioTask{(const char *) "Radio", 128, RADIO_TASK_PRIORITY, prvRadioTask, nullptr};

static PlainCallback radio_tx_init_cb(&radio_tx_init);
static PlainCallback radio_tx_done_cb(&radio_tx_done);

static SemaphoreBinary semaphore(false);

static uint8_t radio_buffer[127];
static uint16_t radio_buffer_len = sizeof(radio_buffer);

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

  for (uint16_t i = 0; i < radio_buffer_len; i++)
  {
    radio_buffer[i] = i;
  }
  
  /* Turn AT86RF215 radio on */
  at86rf215.on();
  
  /* Check AT86RF215 radio */
  status = at86rf215.check();
  if (!status)
  {
      board.error();
  }
  
  /* Set radio callbacks and enable interrupts */
  at86rf215.setTxCallbacks(RADIO_CORE, &radio_tx_init_cb, &radio_tx_done_cb);
  at86rf215.enableInterrupts();
  
  /* Wake up and configure radio */
  at86rf215.wakeup(RADIO_CORE);
  at86rf215.configure(RADIO_CORE, RADIO_SETTINGS, RADIO_FREQUENCY);
  at86rf215.setTransmitPower(RADIO_CORE, RADIO_TX_POWER);
  
  /* Forever */
  while (true)
  {
    uint32_t start_ms, stop_ms;
    int32_t sleep_ms;
    bool sent;
    
    /* Start milliseconds */
    start_ms = Scheduler::get_ms();
    
    /* Load packet to radio */
    at86rf215.loadPacket(RADIO_CORE, radio_buffer, radio_buffer_len);
    
    /* Ready to transmit */
    at86rf215.ready(RADIO_CORE);
    
    /* Transmit packet */
    at86rf215.transmit(RADIO_CORE);
    
    /* Wait until packet has been transmitted or 50 milliseconds */
    sent = semaphore.take(50);
    if (sent == true)
    {
      /* Blink yellow LED */
      led_yellow.on();
      Scheduler::delay_ms(5);
      led_yellow.off();
    }
    else
    {
      /* Blink red LED */
      led_red.on();
      Scheduler::delay_ms(5);
      led_red.off();
    }
        
    /* Calculate sleep time */
    stop_ms = Scheduler::get_ms();
    sleep_ms = 50 - (stop_ms - start_ms);
    
    /* If possible, go to sleep */
    if (sleep_ms > 0)
    {
      /* Delay 25 ms until next packet */
      Scheduler::delay_ms(sleep_ms);
    }
  }
  
  /* Turn AT86RF215 radio off */
  at86rf215.off();
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
  semaphore.giveFromInterrupt();
}
