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

#include "Callback.h"
#include "Scheduler.h"
#include "Semaphore.h"
#include "Task.h"

/*================================ define ===================================*/

#define GREEN_LED_TASK_PRIORITY         	( tskIDLE_PRIORITY + 0 )
#define LIGHT_TASK_PRIORITY             	( tskIDLE_PRIORITY + 1 )
#define TEMPERATURE_TASK_PRIORITY       	( tskIDLE_PRIORITY + 2 )
#define ACCELERATION_TASK_PRIORITY      	( tskIDLE_PRIORITY + 3 )

/*================================ typedef ==================================*/

/*=============================== prototypes ================================*/

extern "C" TickType_t board_sleep(TickType_t xModifiableIdleTime);
extern "C" TickType_t board_wakeup(TickType_t xModifiableIdleTime);

static void prvGreenLedTask(void *pvParameters);
static void prvLightTask(void *pvParameters);
static void prvTemperatureTask(void *pvParameters);
static void prvAccelerationTask(void *pvParameters);

/*=============================== variables =================================*/

/*================================= public ==================================*/

int main (void)
{
    // Initialize board
    board.init();

    // Enable the I2C interface
    i2c.enable();

    // Create the FreeRTOS tasks
    xTaskCreate(prvGreenLedTask, (const char *) "GreenLed", 128, NULL, GREEN_LED_TASK_PRIORITY, NULL);
    xTaskCreate(prvAccelerationTask, (const char *) "Acceleration", 128, NULL, ACCELERATION_TASK_PRIORITY, NULL);
    xTaskCreate(prvTemperatureTask, (const char *) "Temperature", 128, NULL, TEMPERATURE_TASK_PRIORITY, NULL);
    xTaskCreate(prvLightTask, (const char *) "Light", 128, NULL, LIGHT_TASK_PRIORITY, NULL);

    // Start the scheduler
    Scheduler::run();
}

TickType_t board_sleep(TickType_t xModifiableIdleTime)
{
    i2c.sleep();
    return xModifiableIdleTime;
}

TickType_t board_wakeup(TickType_t xModifiableIdleTime)
{
    i2c.wakeup();
    return xModifiableIdleTime;
}

/*================================ private ==================================*/

static void prvTemperatureTask(void *pvParameters)
{
    uint16_t temperature;
    uint16_t humidity;

    if (si7006.isPresent() == true)
    {
        si7006.enable();

        while (true)
        {
            led_orange.on();

            si7006.readTemperature();
            temperature = si7006.getTemperatureRaw();

            si7006.readHumidity();
            humidity = si7006.getHumidityRaw();

            led_orange.off();

            Task::delay(2000);
        }
    }
    else
    {
        led_orange.on();
    }
}

static void prvLightTask(void *pvParameters)
{
    uint16_t light;

    if (max44009.isPresent() == true)
    {
        max44009.enable();

        while (true)
        {
            led_yellow.on();

            max44009.readLux();
            light = max44009.getLuxRaw();

            led_yellow.off();

            Task::delay(2000);
        }
    }
    else
    {
        led_yellow.on();
    }
}

static void prvAccelerationTask(void *pvParameters) {
    uint16_t x, y, z;

    if (adxl34x.isPresent() == true)
    {
        adxl34x.enable();

        while (true)
        {
            led_red.on();

            adxl34x.wakeup();
            adxl34x.readSample(&x, &y, &z);
            adxl34x.suspend();

            led_red.off();

            Task::delay(2000);
        }
    }
    else
    {
        led_red.on();
    }
}

static void prvGreenLedTask(void *pvParameters)
{
    // Forever
    while (true)
    {
        // Turn off green LED for 1950 ms
        led_green.off();
        Task::delay(1950);

        // Turn on green LED for 50 ms
        led_green.on();
        Task::delay(50);
    }
}
