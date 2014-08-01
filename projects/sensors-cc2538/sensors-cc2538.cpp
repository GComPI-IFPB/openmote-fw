/*
 * Copyright 2013 OpenMote Technologies, S.L.
 */

/**
 *
 * @file       sensors-cc2538.cpp
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

/*================================ define ===================================*/

#define GREEN_LED_TASK_PRIORITY         ( tskIDLE_PRIORITY + 0 )
#define LIGHT_TASK_PRIORITY             ( tskIDLE_PRIORITY + 1 )
#define TEMPERATURE_TASK_PRIORITY       ( tskIDLE_PRIORITY + 2 )
#define ACCELERATION_TASK_PRIORITY      ( tskIDLE_PRIORITY + 3 )
#define BUTTON_TASK_PRIORITY            ( tskIDLE_PRIORITY + 3 )

/*================================ typedef ==================================*/

/*=============================== variables =================================*/

static xSemaphoreHandle xSemaphoreButton;

/*=============================== prototypes ================================*/

extern "C" TickType_t board_sleep(TickType_t xModifiableIdleTime);
extern "C" TickType_t board_wakeup(TickType_t xModifiableIdleTime);

static void prvGreenLedTask(void *pvParameters);
static void prvLightTask(void *pvParameters);
static void prvTemperatureTask(void *pvParameters);
static void prvAccelerationTask(void *pvParameters);
static void prvButtonTask(void *pvParameters);

static void button_user_callback(void);

/*================================= public ==================================*/

int main (void)
{
    // Set the TPS62730 in bypass mode (Vin = 3.3V, Iq < 1 uA)
    tps62730_bypass.off();

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

static void button_user_callback(void)
{
    static BaseType_t xHigherPriorityTaskWoken;
    
    xHigherPriorityTaskWoken = pdFALSE;
    
    xSemaphoreGiveFromISR(xSemaphoreButton, &xHigherPriorityTaskWoken);
    
    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}

static void prvButtonTask(void *pvParameters)
{
    xSemaphoreButton = xSemaphoreCreateMutex();

    button_user.setCallback(button_user_callback);
    button_user.enableInterrupt();

    while(true)
    {
        /* The second parameter indicates the interval at which the xSempahore
           is polled and, thus, it determines latency and energy consumption. */
        if (xSemaphoreTake(xSemaphoreButton, (TickType_t) portMAX_DELAY) == pdTRUE)
        {
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
	while(true)
	{
		led_green.on();
		vTaskDelay(50/ portTICK_RATE_MS);
        led_green.off();
		vTaskDelay(1950 / portTICK_RATE_MS);
	}
}

