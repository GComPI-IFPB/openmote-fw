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

#include "BoardImplementation.hpp"
#include "platform_types.h"

#include "Gpio.hpp"
#include "I2c.hpp"
#include "Spi.hpp"

#include "Callback.hpp"
#include "Task.hpp"
#include "Scheduler.hpp"
#include "Semaphore.hpp"

#include "bme280/Bme280.hpp"
#include "opt3001/Opt3001.hpp"

/*================================ define ===================================*/

#define GREEN_LED_TASK_PRIORITY             ( tskIDLE_PRIORITY + 0 )
#define TRANSMIT_TASK_PRIORITY              ( tskIDLE_PRIORITY + 1 )

#define GREEN_LED_TASK_STACK_SIZE           ( 128 )
#define TRANSMIT_TASK_STACK_SIZE            ( 1024 )

#define TX_BUFFER_LENGTH                    ( 127 )
#define EUI48_ADDDRESS_LENGTH               ( 6 )

#define SENSORS_CTRL_PORT                   ( GPIO_A_BASE )
#define SENSORS_CTRL_PIN                    ( GPIO_PIN_7 )

#define BME280_I2C_ADDRESS                  ( BME280_I2C_ADDR_PRIM )
#define OPT3001_I2C_ADDRESS                 ( OPT3001_I2C_ADDR_GND )

/*================================ typedef ==================================*/

typedef struct {
    uint16_t temperature;
    uint16_t humidity;
    uint16_t pressure;
    uint16_t light;
} SensorData;

/*=============================== prototypes ================================*/

extern "C" TickType_t board_sleep(TickType_t xModifiableIdleTime);
extern "C" TickType_t board_wakeup(TickType_t xModifiableIdleTime);

static void prvGreenLedTask(void *pvParameters);
static void prvTransmitTask(void *pvParameters);

static uint16_t prepare_packet(uint8_t* packet_ptr, uint8_t* eui48_address, uint32_t packet_counter, SensorData sensor_data);

static void radio_tx_init(void);
static void radio_tx_done(void);

/*=============================== variables =================================*/

GpioConfig sensors_pwr_cfg = {SENSORS_CTRL_PORT, SENSORS_CTRL_PIN, 0, 0, 0};
GpioOut sensors_pwr_ctrl(sensors_pwr_cfg);

Bme280 bme280(i2c, BME280_I2C_ADDRESS);
Opt3001 opt3001(i2c, OPT3001_I2C_ADDRESS);

PlainCallback radio_tx_init_cb(&radio_tx_init);
PlainCallback radio_tx_done_cb(&radio_tx_done);

SemaphoreBinary semaphore(false);

static bool board_slept;

/*================================= public ==================================*/

int main(void)
{
    // Initialize the board
    board.init();

    // Enable I2C
    i2c.enable();

    // Turn on the sensors board
    sensors_pwr_ctrl.high();

    // Create FreeRTOS tasks
    xTaskCreate(prvGreenLedTask, (const char *) "Green", GREEN_LED_TASK_STACK_SIZE, NULL, GREEN_LED_TASK_PRIORITY, NULL);
    xTaskCreate(prvTransmitTask, (const char *) "Transmit", TRANSMIT_TASK_STACK_SIZE, NULL, TRANSMIT_TASK_PRIORITY, NULL);

    // Start the scheduler
    Scheduler::run();
}

TickType_t board_sleep(TickType_t xModifiableIdleTime)
{
    // Check if board can go to sleep
    if (radio.canSleep() && i2c.canSleep())
    {
        // If so, put Radio and I2C to sleep
        i2c.sleep();
        radio.sleep();

        // Remember that the board went to sleep
        board_slept = true;
    }
    else
    {
        // If not, remember that the board did NOT went to sleep
        board_slept = false;

        // And update the time to ensure it does NOT got to sleep
        xModifiableIdleTime = 0;
    }

    return xModifiableIdleTime;
}

TickType_t board_wakeup(TickType_t xModifiableIdleTime)
{
    // Check if the board went to sleep
    if (board_slept)
    {
        // If so, wakeup Radio and I2C
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

    RadioResult result;

    uint16_t packet_counter = 0;

    // Get EUI48 address
    board.getEUI48(eui48_address);

    // Set radio transmit callbacks
    radio.setTxCallbacks(&radio_tx_init_cb, &radio_tx_done_cb);

    // Enable radio and interrupts
    radio.enable();
    radio.enableInterrupts();

    // Delay for 100 milliseconds
    Scheduler::delay_ms(100);

    // Initialize BME280 and OPT3001 sensors
    bme280.init();
    opt3001.init();
    opt3001.enable();

    // Delay for 100 milliseconds
    Scheduler::delay_ms(100);

    // Forever
    while (true) {
        SensorData sensor_data;
        Bme280Data bme280_data;
        Opt3001Data opt3001_data;
        uint16_t tx_buffer_len;
        bool status;

        // Turn on red LED
        led_red.on();

        // Read temperature, humidity and pressure
        status = bme280.read(&bme280_data);
        if (!status)
        {
            /* Reset BME280 */
            bme280.reset();

            /* Re-initialize BME280 */
            bme280.init();
        }

        // Read light
        status = opt3001.read(&opt3001_data.raw);
        if (status)
        {
            opt3001.convert(opt3001_data.raw, &opt3001_data.lux);
        }

        // Convert sensor data
        if (status)
        {
            // Fill-in sensor data
            sensor_data.temperature = (uint16_t) (bme280_data.temperature * 10.0f);
            sensor_data.humidity    = (uint16_t) (bme280_data.humidity * 10.0f);
            sensor_data.pressure    = (uint16_t) (bme280_data.pressure * 10.0f);
            sensor_data.light       = (uint16_t) (opt3001_data.lux * 10.0f);

            // Prepare radio packet
            tx_buffer_len = prepare_packet(tx_buffer, eui48_address, packet_counter, sensor_data);

            // Turn on radio
            radio.on();

            // Load packet to radio
            result = radio.loadPacket(tx_buffer, tx_buffer_len);
            if (result == RadioResult_Success)
            {
                // Transmit packet
                radio.transmit();

                // Wait until packet has been transmitted
                semaphore.take();
            }

            // Turn off radio
            radio.off();
        }


        // Increment packet counter
        packet_counter++;

        // Turn off red LED
        led_red.off();

        // Delay for 60 seconds
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
    // Turn on orange LED
    led_orange.on();
}

static void radio_tx_done(void)
{
    // Turn off orange LED
    led_orange.off();

    // Notify we have transmitted a packet
    semaphore.giveFromInterrupt();
}

static uint16_t prepare_packet(uint8_t* packet_ptr, uint8_t* eui48_address, uint32_t packet_counter, SensorData sensor_data)
{
    uint16_t packet_length = 0;
    uint8_t i;

    // Copy MAC adress
    for (i = 0; i < EUI48_ADDDRESS_LENGTH; i++)
    {
        packet_ptr[i] = eui48_address[i];
    }
    packet_length = i;

    // Copy packet counter
    i = packet_length;
    packet_ptr[i++] = (uint8_t)((packet_counter & 0xFF000000) >> 24);
    packet_ptr[i++] = (uint8_t)((packet_counter & 0x00FF0000) >> 16);
    packet_ptr[i++] = (uint8_t)((packet_counter & 0x0000FF00) >> 8);
    packet_ptr[i++] = (uint8_t)((packet_counter & 0x000000FF) >> 0);
    packet_length = i;

    // Copy sensor data
    i = packet_length;
    packet_ptr[i++] = (uint8_t) ((sensor_data.temperature & 0xFF00) >> 8);
    packet_ptr[i++] = (uint8_t) ((sensor_data.temperature & 0x00FF) >> 0);
    packet_ptr[i++] = (uint8_t) ((sensor_data.humidity & 0xFF00) >> 8);
    packet_ptr[i++] = (uint8_t) ((sensor_data.humidity & 0x00FF) >> 0);
    packet_ptr[i++] = (uint8_t) ((sensor_data.pressure & 0xFF00) >> 8);
    packet_ptr[i++] = (uint8_t) ((sensor_data.pressure & 0x00FF) >> 0);
    packet_ptr[i++] = (uint8_t) ((sensor_data.light & 0xFF00) >> 8);
    packet_ptr[i++] = (uint8_t) ((sensor_data.light & 0x00FF) >> 0);
    packet_length = i;

    return packet_length;
}
