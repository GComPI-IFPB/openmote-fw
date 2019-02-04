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

#include "FreeRTOS.h"
#include "task.h"

#include "board.h"

#include "Serial.h"

#include "Scheduler.h"
#include "Task.h"

/*================================ define ===================================*/

#define GREEN_LED_TASK_PRIORITY             ( tskIDLE_PRIORITY + 0 )
#define SERIAL_TASK_PRIORITY                ( tskIDLE_PRIORITY + 1 )

#define UART_BAUDRATE                       ( 115200 )

/*================================ typedef ==================================*/

/*=============================== prototypes ================================*/

static void prvGreenLedTask(void *pvParameters);
static void prvSerialTask(void *pvParameters);

/*=============================== variables =================================*/

static uint8_t serial_tx_buffer[] = {'O','p','e','n','M','o','t','e','-','C','C','2','5','3','8'};
static uint8_t* serial_tx_ptr = serial_tx_buffer;
static uint8_t serial_tx_len  = sizeof(serial_tx_buffer);

static uint8_t serial_rx_buffer[128];
static uint8_t* serial_rx_ptr = serial_rx_buffer;
static uint8_t serial_rx_len  = sizeof(serial_rx_buffer);

static Serial serial(uart);

/*================================= public ==================================*/

int main (void)
{
    // Initialize board
    board.init();

    // Enable the UART peripheral and the serial driver
    uart.enable(UART_BAUDRATE);
    serial.init();

    // Create two FreeRTOS tasks
    xTaskCreate(prvGreenLedTask, (const char *) "Green", 128, NULL, GREEN_LED_TASK_PRIORITY, NULL);
    xTaskCreate(prvSerialTask, (const char *) "Serial", 128, NULL, SERIAL_TASK_PRIORITY, NULL);

    // Start the scheduler
    Scheduler::run();
}

/*=============================== protected =================================*/

/*================================ private ==================================*/

static void prvSerialTask(void *pvParameters)
{
    // Turn on red LED
    led_red.on();

    // Print buffer via Serial/UART
    serial.write(serial_tx_ptr, serial_tx_len);

    // Turn off red LED
    led_red.off();

    // Forever
    while (true)
    {
        serial_rx_ptr = serial_rx_buffer;
        serial_rx_len = sizeof(serial_rx_buffer);

        // Read buffer via Serial/UART
        serial_rx_len = serial.read(serial_rx_ptr, serial_rx_len);

        // Delay for 250 ms
        Task::delay(250);

        // Turn on red LED
        led_yellow.on();

        // Write buffer via Serial/UART
        serial.write(serial_rx_ptr, serial_rx_len);

        // Turn off red LED
        led_yellow.off();
    }
}

static void prvGreenLedTask(void *pvParameters)
{
    // Forever
    while (true)
    {
        // Turn off green LED for 950 ms
        led_green.off();
        Task::delay(950);

        // Turn on green LED for 50 ms
        led_green.on();
        Task::delay(50);
    }
}
