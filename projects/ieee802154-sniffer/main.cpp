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

#include <string.h>

#include "FreeRTOS.h"
#include "task.h"
#include "openmote-cc2538.h"

#include "Aes.h"
#include "Gpio.h"
#include "Tps62730.h"
#include "Spi.h"
#include "Enc28j60.h"

#include "Serial.h"
#include "Ethernet.h"

#include "Callback.h"
#include "Scheduler.h"
#include "Task.h"

#include "SnifferEthernet.h"
#include "SnifferSerial.h"

/*================================ define ===================================*/

#define GREEN_LED_TASK_PRIORITY             ( tskIDLE_PRIORITY + 0 )
#define SERIAL_TASK_PRIORITY                ( tskIDLE_PRIORITY + 1 )
#define SNIFFER_TASK_PRIORITY               ( tskIDLE_PRIORITY + 2 )

#define SNIFFER_DEFAULT_CHANNEL             ( 26 )

#define SERIAL_CHANGE_RADIO_CHANNEL_CMD     ( 0xCC )
#define SERIAL_CHANGE_AES_KEY_CMD           ( 0xCE )

#define SNIFFER_ETHERNET                    ( 0 )
#define SNIFFER_SERIAL                      ( 1 )
#define SNIFFER_TYPE                        ( SNIFFER_SERIAL )

/*================================ typedef ==================================*/

/*=============================== prototypes ================================*/

static void prvGreenLedTask(void *pvParameters);
static void prvSnifferTask(void *pvParameters);
static void prvSerialTask(void *pvParamters);

static void changeRadioChannel(uint8_t channel);
static void changeAesKey(bool enable, uint8_t* key);

/*=============================== variables =================================*/

static Serial serial(uart);
static Ethernet ethernet(enc28j60);

#if (SNIFFER_TYPE == SNIFFER_SERIAL)
static SnifferSerial   sniffer(board, radio, aes, serial);
#elif  (SNIFFER_TYPE == SNIFFER_ETHERNET)
static SnifferEthernet sniffer(board, radio, aes, ethernet);
#else
#error "SNIFFER_TYPE not defined or not valid!"
#endif

static uint8_t serial_buffer[32];
static uint8_t* serial_buffer_ptr;
static int32_t serial_buffer_len;

/*================================= public ==================================*/

int main(void)
{
    // Set the TPS62730 in bypass mode (Vin = 3.3V, Iq < 1 uA)
    tps62730.setBypass();

    // Enable the SPI peripheral
    spi.enable();

    // Enable the UART peripheral
    uart.enable();

    // Init the serial
    serial.init();

    // Create the blink task
    xTaskCreate(prvGreenLedTask, (const char *) "LedTask", 128, NULL, GREEN_LED_TASK_PRIORITY, NULL);

    // Create the sniffer task to process packets
    xTaskCreate(prvSnifferTask, (const char *) "SnifferTask", 128, NULL, SNIFFER_TASK_PRIORITY, NULL);

    // Create the serial task to receive commands
    xTaskCreate(prvSerialTask, (const char *) "SerialTask", 128, NULL, SERIAL_TASK_PRIORITY, NULL);

    // Start the scheduler
    Scheduler::run();
}

/*================================ private ==================================*/

static void prvGreenLedTask(void *pvParameters)
{
    // Forever
    while (true)
    {
        // Turn off the green LED and keep it for 950 ms
        led_green.off();
        Task::delay(950);

        // Turn on the green LED and keep it for 50 ms
        led_green.on();
        Task::delay(50);
    }
}

static void prvSerialTask(void *pvParamters)
{
    while (true)
    {
        // Reset the buffer pointer and length
        serial_buffer_ptr = serial_buffer;
        serial_buffer_len = sizeof(serial_buffer);

        // Wait until we receive a command
        serial_buffer_len = serial.read(serial_buffer_ptr, serial_buffer_len);

        // Get the command and decide the action
        switch (serial_buffer[0]) {
            case SERIAL_CHANGE_RADIO_CHANNEL_CMD:
                changeRadioChannel(serial_buffer[1]);
                break;
            case SERIAL_CHANGE_AES_KEY_CMD:
                changeAesKey(serial_buffer[1], &serial_buffer[2]);
                break;
            default:
                break;
        }
    }
}

static void prvSnifferTask(void *pvParameters)
{
    // Initialize the sniffer
    sniffer.init();

    // Set the default sniffer channel
    sniffer.setChannel(SNIFFER_DEFAULT_CHANNEL);

    while (true)
    {
        // Start the sniffer
        sniffer.start();

        // Process a frame
        sniffer.processRadioFrame();
    }
}


static void changeRadioChannel(uint8_t channel)
{
    // Stop the sniffer prior to updating the channel
    sniffer.stop();

    // Change the sniffer channel
    sniffer.setChannel(channel);

    // Re-start the sniffer
    sniffer.start();
}

static void changeAesKey(bool enable, uint8_t* key) {

}