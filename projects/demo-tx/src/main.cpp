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
#include "Spi.h"
#include "I2c.h"

#include "Callback.h"
#include "Scheduler.h"
#include "Semaphore.h"
#include "Task.h"

#include "bme280/Bme280.h"

/*================================ define ===================================*/

#define GREEN_LED_TASK_PRIORITY             ( tskIDLE_PRIORITY + 0 )
#define TRANSMIT_TASK_PRIORITY              ( tskIDLE_PRIORITY + 1 )

#define GREEN_LED_TASK_STACK_SIZE           ( 128 )
#define TRANSMIT_TASK_STACK_SIZE            ( 1024 )

#define TX_BUFFER_LENGTH                    ( 125 )
#define EUI48_ADDDRESS_LENGTH               ( 6 )

#define BME280_I2C_ADDRESS                  ( 0x00 )

/*================================ typedef ==================================*/

/*=============================== prototypes ================================*/

extern "C" TickType_t board_sleep(TickType_t xModifiableIdleTime);
extern "C" TickType_t board_wakeup(TickType_t xModifiableIdleTime);

static void prvGreenLedTask(void *pvParameters);
static void prvTransmitTask(void *pvParameters);

static uint16_t prepare_packet(uint8_t* packet_ptr, uint8_t* eui48_address, uint32_t packet_counter, uint16_t temperature, uint16_t pressure, uint16_t humidity);

static void radio_tx_init(void);
static void radio_tx_done(void);

/*=============================== variables =================================*/

PlainCallback radio_tx_init_cb(&radio_tx_init);
PlainCallback radio_tx_done_cb(&radio_tx_done);

SemaphoreBinary semaphore(false);

Bme280 bme280(i2c, BME280_I2C_ADDRESS);

/*================================= public ==================================*/

int main(void)
{
    // Initialize the board
    board.init();

    // Enable I2C
    i2c.enable();

    // Create FreeRTOS tasks
    xTaskCreate(prvGreenLedTask, (const char *) "Green", GREEN_LED_TASK_STACK_SIZE, NULL, GREEN_LED_TASK_PRIORITY, NULL);
    xTaskCreate(prvTransmitTask, (const char *) "Transmit", TRANSMIT_TASK_STACK_SIZE, NULL, TRANSMIT_TASK_PRIORITY, NULL);

    // Start the scheduler
    Scheduler::run();
}

static bool slept;

TickType_t board_sleep(TickType_t xModifiableIdleTime)
{
    slept = false;

    /* Check if radio is active */
    if (radio.canSleep() && i2c.canSleep())
    {
        i2c.sleep();
        radio.sleep();
        slept = true;
        return xModifiableIdleTime;
    }

    return 0;
}

TickType_t board_wakeup(TickType_t xModifiableIdleTime)
{
    if (slept)
    {
        radio.wakeup();
        i2c.wakeup();
    }
    return xModifiableIdleTime;
}

/*================================ private ==================================*/

static void prvTransmitTask(void *pvParameters)
{
    uint8_t tx_buffer[TX_BUFFER_LENGTH];
    uint8_t eui48_address[EUI48_ADDDRESS_LENGTH];
    uint16_t packet_counter;

    RadioResult result;

    /* Get EUI48 address */
    board.getEUI48(eui48_address);

    /* Set radio transmit callbacks */
    radio.setTxCallbacks(&radio_tx_init_cb, &radio_tx_done_cb);

    /* Enable radio */
    radio.enable();
    radio.enableInterrupts();

    /* Initialize BME280 sensor */
    bme280.init();

    // Delay for 100 milliseconds
    Scheduler::delay_ms(100);

    packet_counter = 0;

    // Forever
    while (true) {
        Bme280Data data;
        uint16_t tx_buffer_len;

        // Turn on red LED
        led_red.on();

        /* Read temperature, humidity and pressure */
        bme280.read(&data);
        uint16_t temperature = (uint16_t) (data.temperature * 10.0f);
        uint16_t humidity = (uint16_t) (data.humidity * 10.0f);
        uint16_t pressure = (uint16_t) (data.pressure * 10.0f);

        /* Prepare packet */
        tx_buffer_len = prepare_packet(tx_buffer, eui48_address, packet_counter, temperature, pressure, humidity);

        /* Turn on radio */
        radio.on();

        /* Load packet to radio */
        result = radio.loadPacket(tx_buffer, tx_buffer_len);
        if (result == RadioResult_Success)
        {
            /* Transmit packet */
            radio.transmit();

            /* Wait until packet has been transamitted */
            semaphore.take();
        }

        /* Turn off radio */
        radio.off();

        /* Increment packet counter */
        packet_counter++;

        // Turn off red LED
        led_red.off();

        // Delay for 10 seconds
        Scheduler::delay_ms(10000);
    }
}

static void prvGreenLedTask(void *pvParameters)
{
    // Forever
    while (true) {
        // Turn off green LED for 9999 ms
        led_green.off();
        Scheduler::delay_ms(999);

        // Turn on green LED for 1 ms
        led_green.on();
        Scheduler::delay_ms(1);
    }
}

static void radio_tx_init(void)
{
    led_orange.on();
}

static void radio_tx_done(void)
{
    led_orange.off();
    semaphore.giveFromInterrupt();
}

static uint16_t prepare_packet(uint8_t* packet_ptr, uint8_t* eui48_address, uint32_t packet_counter, uint16_t temperature, uint16_t pressure, uint16_t humidity)
{
    uint16_t packet_length = 0;
    uint8_t i;

    /* Copy MAC adress */
    for (i = 0; i < EUI48_ADDDRESS_LENGTH; i++)
    {
        packet_ptr[i] = eui48_address[i];
    }
    packet_length = i;

    /* Copy packet counter */
    i = packet_length;
    packet_ptr[i++] = (uint8_t)((packet_counter & 0xFF000000) >> 24);
    packet_ptr[i++] = (uint8_t)((packet_counter & 0x00FF0000) >> 16);
    packet_ptr[i++] = (uint8_t)((packet_counter & 0x0000FF00) >> 8);
    packet_ptr[i++] = (uint8_t)((packet_counter & 0x000000FF) >> 0);
    packet_length = i;

    /* Copy sensor data */
    i = packet_length;
    packet_ptr[i++] = (uint8_t) ((temperature & 0xFF00) >> 8);
    packet_ptr[i++] = (uint8_t) ((temperature & 0x00FF) >> 0);
    packet_ptr[i++] = (uint8_t) ((humidity & 0xFF00) >> 8);
    packet_ptr[i++] = (uint8_t) ((humidity & 0x00FF) >> 0);
    packet_ptr[i++] = (uint8_t) ((pressure & 0xFF00) >> 8);
    packet_ptr[i++] = (uint8_t) ((pressure & 0x00FF) >> 0);
    packet_length = i;

    return packet_length;
}
