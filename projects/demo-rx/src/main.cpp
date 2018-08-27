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
#include "platform_types.h"

#include "Board.h"
#include "Gpio.h"
#include "Uart.h"

#include "Serial.h"

#include "Callback.h"
#include "Scheduler.h"
#include "Semaphore.h"
#include "Task.h"

/*================================ define ===================================*/

#define GREEN_LED_TASK_PRIORITY             ( tskIDLE_PRIORITY + 0 )
#define RECEIVE_TASK_PRIORITY               ( tskIDLE_PRIORITY + 1 )

#define GREEN_LED_TASK_STACK_SIZE           ( 128 )
#define RECEIVE_TASK_STACK_SIZE             ( 1024 )

#define SERIAL_BUFFER_LENGTH                ( 256 )

/*================================ typedef ==================================*/

/*=============================== prototypes ================================*/

static void prvGreenLedTask(void *pvParameters);
static void prvReceiveTask(void *pvParameters);

static void radio_rx_init(void);
static void radio_rx_done(void);

static uint16_t prepare_serial(uint8_t* buffer_ptr, uint8_t* packet_ptr, uint16_t packet_length, int8_t rssi);

/*=============================== variables =================================*/

PlainCallback radio_rx_init_cb(&radio_rx_init);
PlainCallback radio_rx_done_cb(&radio_rx_done);

static SemaphoreBinary semaphore(false);

static Serial serial(uart);

static uint8_t serial_buffer[SERIAL_BUFFER_LENGTH];

/*================================= public ==================================*/

int main(void)
{
    // Initialize the board
    board.init();

    // Initialize UART
    uart.enable();

    // Init the serial
    serial.init();

    // Create FreeRTOS tasks
    xTaskCreate(prvGreenLedTask, (const char *) "Green", GREEN_LED_TASK_STACK_SIZE, NULL, GREEN_LED_TASK_PRIORITY, NULL);
    xTaskCreate(prvReceiveTask, (const char *) "Receive", RECEIVE_TASK_STACK_SIZE, NULL, RECEIVE_TASK_PRIORITY, NULL);

    // Start the scheduler
    Scheduler::run();
}

/*================================ private ==================================*/

float temperature;
float humidity;
float pressure;

static void prvReceiveTask(void *pvParameters)
{
    static uint8_t rx_buffer[127];

    int8_t rssi;
    uint8_t lqi;
    bool crc;

    RadioResult result;

    radio.setRxCallbacks(&radio_rx_init_cb, &radio_rx_done_cb);

    radio.enable();
    radio.enableInterrupts();

    // Forever
    while (true) {
        // Turn on red LED
        led_red.on();

        radio.on();

        radio.receive();
        if (semaphore.take())
        {
            uint8_t* radioBuffer_ptr;
            uint8_t radioBuffer_len;

            radioBuffer_ptr = rx_buffer;
            radioBuffer_len = sizeof(rx_buffer);

            // Get packet from the radio
            result = radio.getPacket(radioBuffer_ptr, &radioBuffer_len, &rssi, &lqi, &crc);

            // If reception is sucessful and packet CRC matches
            if (result == RadioResult_Success && crc == true)
            {
                uint16_t length;

                radio.off();

                // Turn on yellow LED
                led_yellow.on();

                // Prepare serial buffer
                length = prepare_serial(serial_buffer, radioBuffer_ptr, radioBuffer_len, rssi);

                // Transmit the radio frame over Serial
                serial.write(serial_buffer, length);

                // Delay for 1 millisecond
                vTaskDelay(1 / portTICK_RATE_MS);

                // Turn off yellow LED
                led_yellow.off();
            }
        }

        // Turn off red LED
        led_red.off();
    }
}

static void prvGreenLedTask(void *pvParameters)
{
    // Forever
    while (true) {
        // Turn off green LED for 999 ms
        led_green.off();
        vTaskDelay(999 / portTICK_RATE_MS);

        // Turn on green LED for 1 ms
        led_green.on();
        vTaskDelay(1 / portTICK_RATE_MS);
    }
}

static void radio_rx_init(void)
{
    led_orange.on();
}

static void radio_rx_done(void)
{
    led_orange.off();
    semaphore.giveFromInterrupt();
}

static uint16_t prepare_serial(uint8_t* buffer_ptr, uint8_t* packet_ptr, uint16_t packet_length, int8_t rssi)
{
    uint16_t length;

    // Copy radio packet payload
    for (uint8_t i = 0; i < packet_length; i++)
    {
        buffer_ptr[i] = packet_ptr[i];
    }

    // Update buffer length
    length = packet_length;

    // Copy RSSI value
    buffer_ptr[length++] = rssi;

    return length;
}
