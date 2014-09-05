/*
 * Copyright 2013 OpenMote Technologies, S.L.
 */

/**
 *
 * @file       main.cpp
 * @author     Pere Tuset-Peiro (peretuset@openmote.com)
 * @version    v0.1
 * @date       May, 2014
 * @brief
 * @ingroup
 *
 */

/*================================ include ==================================*/

#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"

#include "openmote-cc2538.h"

#include "Callback.h"

/*================================ define ===================================*/

#define GREEN_LED_TASK_PRIORITY         ( tskIDLE_PRIORITY + 0 )
#define LIGHT_TASK_PRIORITY             ( tskIDLE_PRIORITY + 1 )
#define TEMPERATURE_TASK_PRIORITY       ( tskIDLE_PRIORITY + 2 )
#define ACCELERATION_TASK_PRIORITY      ( tskIDLE_PRIORITY + 3 )
#define BUTTON_TASK_PRIORITY            ( tskIDLE_PRIORITY + 3 )

/*================================ typedef ==================================*/

/*=============================== prototypes ================================*/

extern "C" TickType_t board_sleep(TickType_t xModifiableIdleTime);
extern "C" TickType_t board_wakeup(TickType_t xModifiableIdleTime);

static void prvGreenLedTask(void *pvParameters);
static void prvLightTask(void *pvParameters);
static void prvTemperatureTask(void *pvParameters);
static void prvAccelerationTask(void *pvParameters);
static void prvButtonTask(void *pvParameters);

static void buttonCallback(void);

/*=============================== variables =================================*/

static xSemaphoreHandle buttonSemaphore;
static GenericCallback userCallback(buttonCallback);

/*================================= public ==================================*/

int main (void)
{
    // Set the TPS62730 in bypass mode (Vin = 3.3V, Iq < 1 uA)
    tps62730.setBypass();

    // Enable the I2C interface
    i2c.enable(I2C_BAUDRATE);

    // Create the FreeRTOS tasks
    xTaskCreate(prvGreenLedTask, (const char *) "GreenLed", 128, NULL, GREEN_LED_TASK_PRIORITY, NULL);
    xTaskCreate(prvAccelerationTask, (const char *) "Acceleration", 128, NULL, ACCELERATION_TASK_PRIORITY, NULL);
    xTaskCreate(prvTemperatureTask, (const char *) "Temperature", 128, NULL, TEMPERATURE_TASK_PRIORITY, NULL);
    xTaskCreate(prvLightTask, (const char *) "Light", 128, NULL, LIGHT_TASK_PRIORITY, NULL);
    xTaskCreate(prvButtonTask, (const char *) "Button", 128, NULL, BUTTON_TASK_PRIORITY, NULL);

    // Kick the FreeRTOS scheduler
    vTaskStartScheduler();
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

static void buttonCallback(void)
{
    // Determines if the interrupt triggers a context switch
    static BaseType_t xHigherPriorityTaskWoken;
    xHigherPriorityTaskWoken = pdFALSE;

    // Give the button semaphore as the button has been pressed
    xSemaphoreGiveFromISR(buttonSemaphore, &xHigherPriorityTaskWoken);

    // Force a context switch after the interrupt if required
    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}

static void prvButtonTask(void *pvParameters)
{
    // Create the button semaphore
    buttonSemaphore = xSemaphoreCreateMutex();

    // Configure the user button
    button_user.setCallback(&userCallback);
    button_user.enableInterrupt();

    // Forever
    while (true)
    {
        // Take the buttonSemaphore, block until available
        if (xSemaphoreTake(buttonSemaphore, (TickType_t) portMAX_DELAY) == pdTRUE)
        {
            // Toggle the red LED
            led_red.toggle();
        }
    }
}

static void prvTemperatureTask(void *pvParameters)
{
    uint16_t temperature;
    uint16_t humidity;

    vTaskDelay(500 / portTICK_RATE_MS);

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

            vTaskDelay(2000 / portTICK_RATE_MS);
        }
    }
    else
    {
        led_red.on();
        vTaskDelete(NULL);
    }
}

static void prvLightTask(void *pvParameters)
{
    uint16_t light;

    vTaskDelay(500 / portTICK_RATE_MS);

    if (max44009.isPresent() == true)
    {

        max44009.enable();

        while(true)
        {
            led_orange.on();

            max44009.readLux();
            light = max44009.getLuxRaw();

            led_orange.off();

            vTaskDelay(2000 / portTICK_RATE_MS);
        }
    }
    else
    {
        led_red.on();
        vTaskDelete(NULL);
    }
}

static void prvAccelerationTask(void *pvParameters) {
    uint16_t x, y, z;

    vTaskDelay(500 / portTICK_RATE_MS);

    if (adxl346.isPresent() == true)
    {
        adxl346.enable();

        while(true)
        {
            led_orange.on();

            adxl346.readAcceleration();
            x = adxl346.getX();
            y = adxl346.getY();
            z = adxl346.getZ();

            led_orange.off();

            vTaskDelay(2000 / portTICK_RATE_MS);
        }
    }
    else
    {
        led_red.on();
        vTaskDelete(NULL);
    }
}

static void prvGreenLedTask(void *pvParameters)
{
    // Forever
    while(true)
    {
        // Turn off green LED for 1950 ms
        led_green.off();
        vTaskDelay(1950 / portTICK_RATE_MS);

        // Turn on green LED for 50 ms
        led_green.on();
        vTaskDelay(50 / portTICK_RATE_MS);
    }
}

