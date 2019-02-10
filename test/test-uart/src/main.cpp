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

#include "Callback.hpp"
#include "Scheduler.hpp"
#include "Semaphore.hpp"
#include "Task.hpp"

/*================================ define ===================================*/

#define GREEN_LED_TASK_PRIORITY             ( tskIDLE_PRIORITY + 0 )
#define UART_TASK_PRIORITY                  ( tskIDLE_PRIORITY + 1 )

#define UART_BAUDRATE                       ( 2000000 )
#define UART_BUFFER_LENGTH                  ( 1024 )

/*================================ typedef ==================================*/

typedef struct
{
  uint8_t* head;
  uint16_t length;
  uint16_t available;
  uint8_t* ptr;
  uint16_t count;
} uart_data_t;

/*=============================== prototypes ================================*/

static void prvGreenLedTask(void *pvParameters);
static void prvUartTask(void *pvParameters);

static void uart_rx(void);
static void uart_tx(void);

/*=============================== variables =================================*/

static Task heartbeatTask{(const char *) "Green", 128, GREEN_LED_TASK_PRIORITY, prvGreenLedTask, nullptr};
static Task uartTask{(const char *) "Uart", 128, UART_TASK_PRIORITY, prvUartTask, nullptr};

static PlainCallback uart_rx_cb(&uart_rx);
static PlainCallback uart_tx_cb(&uart_tx);

static uint8_t uart_tx_buffer[UART_BUFFER_LENGTH];
static uint8_t uart_rx_buffer[UART_BUFFER_LENGTH];

static uart_data_t uart_data_tx {.head = uart_tx_buffer, .length = sizeof(uart_tx_buffer)};
static uart_data_t uart_data_rx {.head = uart_rx_buffer, .length = sizeof(uart_rx_buffer)};

/*================================= public ==================================*/

int main (void)
{
  /* Initialize the board */
  board.init();

  /* Enable the UART peripheral */
  uart.enable(UART_BAUDRATE);
  uart.setRxCallback(&uart_rx_cb);
  uart.setTxCallback(&uart_tx_cb);
  uart.enableDMA();
  uart.enableInterrupts();

  /* Start the scheduler */
  Scheduler::run();
}

/*=============================== protected =================================*/

/*================================ private ==================================*/

static void prvUartTask(void *pvParameters)
{
  bool copy = true;
    
  /* Forever */
  while (true)
  {
    /* Restore uart_data_rx structure */
    uart_data_rx.ptr = uart_data_rx.head;
    uart_data_rx.count = 0;
    uart_data_rx.available = uart_data_rx.length;

    /* Restore uart_data_tx structure */
    uart_data_tx.ptr = uart_data_tx.head;
    uart_data_tx.count = 0;
    uart_data_tx.available = uart_data_tx.length;    

    /* Wait until a packet has been received from UART */
    uart.rxLock();

    /* Copy UART receive buffer */
    if (copy)
    {
      /* Copy UART receive buffer to UART transmit buffer using DMA */
      uart_data_tx.length = dma.memcpy(uart_data_tx.head, uart_data_rx.head, uart_data_rx.count);

      /* Send buffer to UART using DMA */
      uart.writeByte(uart_data_tx.head, uart_data_tx.length);
    }
    else
    {
      /* Send buffer to UART using DMA */
      uart.writeByte(uart_data_rx.head, uart_data_rx.count);
    }
    
    /* Wait until packet has been transmitted from UART */
    uart.txLock();
  }
}

static void prvGreenLedTask(void *pvParameters)
{
  /* Forever */
  while (true)
  {
    /* Turn off green LED for 990 ms */
    led_green.off();
    Scheduler::delay_ms(990);

    /* Turn on green LED for 10 ms */
    led_green.on();
    Scheduler::delay_ms(10);
  }
}

static void uart_rx(void)
{
  uint32_t count;
  bool timeout;
  
  /* Check if the UART has timed out */
  timeout = uart.readTimeout();
    
  if (timeout)
  {
    /* If so, read all remaining bytes */
    count = uart.readByte(uart_data_rx.ptr, 16);
  }
  else
  {
    /* Otherwise, read all bytes except the last one */
    count = uart.readByte(uart_data_rx.ptr, 15);
  }

  /* Increment pointer, count and available variables */
  uart_data_rx.ptr       += count;
  uart_data_rx.count     += count;
  uart_data_rx.available -= count;
  
  /* If the UART has timed out */
  if (timeout)
  {
    /* Give the RX semaphore to signal end of reception */
    uart.rxUnlockFromInterrupt();
  }
}

static void uart_tx(void)
{
  /* Give the TX semaphore to signal end of transmission */
  uart.txUnlockFromInterrupt();
}