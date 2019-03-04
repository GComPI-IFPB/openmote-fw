/**
 * @file       main.cpp
 * @author     Pere Tuset-Peiro (peretuset@openmote.com)
 * @version    v0.1
 * @date       May, 2015
 * @brief
 *
 * @copyright  Copyright 2015, OpenMote Technologies, S.L.
 *             This file is licensed under the GNU General Public License v2.
 */

/*================================ include ==================================*/

#include "BoardImplementation.hpp"

#include "Callback.hpp"
#include "Scheduler.hpp"
#include "Semaphore.hpp"
#include "Task.hpp"

#include "Serial.hpp"

/*================================ define ===================================*/

#define GREEN_LED_TASK_PRIORITY             ( tskIDLE_PRIORITY + 0 )
#define SERIAL_TASK_PRIORITY                ( tskIDLE_PRIORITY + 1 )

#define UART_BAUDRATE                       ( 2304000 )

/*================================ typedef ==================================*/

/*=============================== prototypes ================================*/

static void prvGreenLedTask(void *pvParameters);
static void prvSerialTask(void *pvParameters);

/*=============================== variables =================================*/

static Task heartbeatTask{(const char *) "Green", 128, GREEN_LED_TASK_PRIORITY, prvGreenLedTask, nullptr};
static Task serialTask{(const char *) "Serial", 128, SERIAL_TASK_PRIORITY, prvSerialTask, nullptr};

static Serial serial(uart0);

static uint8_t serial_tx_buffer[1024];
static uint8_t* serial_tx_ptr = serial_tx_buffer;
static uint16_t serial_tx_len  = sizeof(serial_tx_buffer);

static uint8_t serial_rx_buffer[1024];
static uint8_t* serial_rx_ptr = serial_rx_buffer;
static uint16_t serial_rx_len  = sizeof(serial_rx_buffer);

/*================================= public ==================================*/

int main (void)
{
  /* Initialize the board */
  board.init();

  /* Enable the UART interface */
  uart0.enable(UART_BAUDRATE);
  
  /* Initialize Serial interface */
  serial.init();

  /* Start the scheduler */
  Scheduler::run();
}

/*=============================== protected =================================*/

/*================================ private ==================================*/

static void prvSerialTask(void *pvParameters)
{
  /* Forever */
  while (true)
  {
    /* Restore serial_rx pointer and length */
    serial_rx_ptr = serial_rx_buffer;
    serial_rx_len = sizeof(serial_rx_buffer);

    /* Read buffer using Serial */
    serial_rx_len = serial.read(serial_rx_ptr, serial_rx_len);
    
    /* If we have received a message */
    if (serial_rx_len > 0)
    {
      /* Write buffer via Serial */
      serial.write(serial_rx_ptr, serial_rx_len, true);
    }
  }
}

static void prvGreenLedTask(void *pvParameters)
{
  /* Forever */
  while (true)
  {
    /* Turn off green LED for 950 ms */
    led_green.off();
    Scheduler::delay_ms(950);

    /* Turn on green LED for 50 ms */
    led_green.on();
    Scheduler::delay_ms(50);
  }
}
