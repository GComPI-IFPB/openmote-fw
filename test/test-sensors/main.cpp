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

#include "openmote-cc2538.h"

#include "Board.h"
#include "Gpio.h"
#include "I2c.h"
#include "Radio.h"
#include "TemperatureSensor.h"

#include "Adxl346.h"
#include "Max44009.h"
#include "Sht21.h"
#include "Tps62730.h"

#include "Callback.h"
#include "Scheduler.h"
#include "Semaphore.h"
#include "Task.h"

/*================================ define ===================================*/

#define GREEN_LED_TASK_PRIORITY         	( tskIDLE_PRIORITY + 0 )
#define LIGHT_TASK_PRIORITY             	( tskIDLE_PRIORITY + 1 )
#define TEMPERATURE_TASK_PRIORITY       	( tskIDLE_PRIORITY + 2 )
#define TEMPERATURE_INTERNAL_TASK_PRIORITY	( tskIDLE_PRIORITY + 0 )
#define ACCELERATION_TASK_PRIORITY      	( tskIDLE_PRIORITY + 3 )

/*================================ typedef ==================================*/

/*=============================== prototypes ================================*/

extern "C" TickType_t board_sleep(TickType_t xModifiableIdleTime);
extern "C" TickType_t board_wakeup(TickType_t xModifiableIdleTime);

static void prvGreenLedTask(void *pvParameters);
static void prvLightTask(void *pvParameters);
static void prvTemperatureTask(void *pvParameters);
static void prvTemperatureInternalTask(void *pvParameters);
static void prvAccelerationTask(void *pvParameters);

/*=============================== variables =================================*/

/*================================= public ==================================*/

int main (void)
{
    // Set the TPS62730 in bypass mode (Vin = 3.3V, Iq < 1 uA)
    tps62730.setBypass();

    // Enable the I2C interface
    i2c.enable();

    // Create the FreeRTOS tasks
    xTaskCreate(prvGreenLedTask, (const char *) "GreenLed", 128, NULL, GREEN_LED_TASK_PRIORITY, NULL);
    xTaskCreate(prvAccelerationTask, (const char *) "Acceleration", 128, NULL, ACCELERATION_TASK_PRIORITY, NULL);
    xTaskCreate(prvTemperatureTask, (const char *) "Temperature", 128, NULL, TEMPERATURE_TASK_PRIORITY, NULL);
    xTaskCreate(prvLightTask, (const char *) "Light", 128, NULL, LIGHT_TASK_PRIORITY, NULL);
    xTaskCreate(prvTemperatureInternalTask, (const char *) "TemperatureInternal", 128, NULL, TEMPERATURE_INTERNAL_TASK_PRIORITY, NULL);

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

    if (sht21.isPresent() == true)
    {
        sht21.enable();

        while(true)
        {
            led_orange.on();

            sht21.readTemperature();
            temperature = sht21.getTemperatureRaw();

            sht21.readHumidity();
            humidity = sht21.getHumidityRaw();

            led_orange.off();

            Task::delay(2000);
        }
    }
    else
    {
        led_orange.on();
        Task::remove();
    }
}

static void prvTemperatureInternalTask(void *pvParameters)
{
    uint16_t temperature;
    
    temp.enable();

    while(true)
    {
        led_orange.on();

        temp.readTemperature();
        temperature = temp.getTemperatureRaw();

        led_orange.off();

        Task::delay(2000);
    }
}

static void prvLightTask(void *pvParameters)
{
    uint16_t light;

    if (max44009.isPresent() == true)
    {
        max44009.enable();

        while(true)
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
        Task::remove();
    }
}

static void prvAccelerationTask(void *pvParameters) {
    uint16_t x, y, z;

    if (adxl346.isPresent() == true)
    {
        adxl346.enable();

        while(true)
        {
            led_red.on();

            adxl346.wakeup();
            adxl346.readSample(&x, &y, &z);
            adxl346.suspend();

            led_red.off();

            Task::delay(2000);
        }
    }
    else
    {
        led_red.on();
        Task::remove();
    }
}

static void prvGreenLedTask(void *pvParameters)
{
    // Forever
    while(true)
    {
        // Turn off green LED for 1950 ms
        led_green.off();
        Task::delay(1950);

        // Turn on green LED for 50 ms
        led_green.on();
        Task::delay(50);
    }
}

