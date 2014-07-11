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
#define BUTTON_TASK_PRIORITY            ( tskIDLE_PRIORITY + 3 )

/*================================ typedef ==================================*/

/*=============================== variables =================================*/

static xSemaphoreHandle xSemaphoreButton;

/*=============================== prototypes ================================*/

extern "C" void board_sleep(TickType_t xModifiableIdleTicks);
extern "C" void board_wakeup(TickType_t xModifiableIdleTicks);

static void prvGreenLedTask(void *pvParameters);
static void prvLightTask(void *pvParameters);
static void prvTemperatureTask(void *pvParameters);
static void prvButtonTask(void *pvParameters);
static void button_user_callback(void);

/*================================= public ==================================*/

/**
 *
 */
int main (void) {
    tps62730_bypass.off();

    button_user.setCallback(button_user_callback);
    button_user.enableInterrupt();
    
    // Enable the UART interface
    uart.enable(UART_BAUDRATE, UART_CONFIG, UART_INT_MODE);
    
    // Enable the I2C interface
    i2c.enable(I2C_BAUDRATE);
    
    xSemaphoreButton = xSemaphoreCreateMutex();
    
    xTaskCreate(prvGreenLedTask, ( const char * ) "GreenLed", 128, NULL, GREEN_LED_TASK_PRIORITY, NULL );
    xTaskCreate(prvTemperatureTask, ( const char * ) "Temperature", 128, NULL, TEMPERATURE_TASK_PRIORITY, NULL );
    xTaskCreate(prvLightTask, ( const char * ) "Light", 128, NULL, LIGHT_TASK_PRIORITY, NULL );
    xTaskCreate(prvButtonTask, ( const char * ) "Button", 128, NULL, BUTTON_TASK_PRIORITY, NULL );

    vTaskStartScheduler();
}

/*================================ private ==================================*/

static void button_user_callback(void) {
    static BaseType_t xHigherPriorityTaskWoken;
    xHigherPriorityTaskWoken = pdFALSE;
    xSemaphoreGiveFromISR( xSemaphoreButton, &xHigherPriorityTaskWoken );
    portYIELD_FROM_ISR( xHigherPriorityTaskWoken );
}

static void prvButtonTask( void *pvParameters ) {
    while(true) {
        /* The second parameter indicates the interval at which the xSempahore
           is polled and, thus, it determines latency and energy consumption. */
        if (xSemaphoreTake( xSemaphoreButton, ( TickType_t ) portMAX_DELAY ) == pdTRUE) {
            led_red.toggle();
        }
    } 
}

static void prvTemperatureTask( void *pvParameters ) {
    uint16_t temperature;
    uint16_t humidity;

    vTaskDelay(500 / portTICK_RATE_MS);

    if (sht21.isPresent() == true) {
    	while(true) {
    	    led_orange.on();
    	    sht21.readTemperature();
    	    sht21.readHumidity();
            temperature = sht21.getTemperatureRaw();
            humidity = sht21.getHumidityRaw();
            led_orange.off();
            vTaskDelay(2000 / portTICK_RATE_MS);
	    }
	} else {
	    led_red.off();
	    i2c.sleep();
	    vTaskDelete( NULL );
	}
}

static void prvLightTask( void *pvParameters ) {
    uint16_t light;
    
    vTaskDelay(500 / portTICK_RATE_MS);
    
    if (max44009.isPresent() == true) {
    	while(true) {
        	led_orange.on();
        	max44009.readLux();
        	light = max44009.getLuxRaw();
           	led_orange.off();
            vTaskDelay(2000 / portTICK_RATE_MS);
	    }
	} else {
	    led_red.off();
	    i2c.sleep();
	    vTaskDelete( NULL );
	}
}

static void prvGreenLedTask( void *pvParameters ) {
	while(true) {
		led_green.on();
		vTaskDelay(50/ portTICK_RATE_MS);
        led_green.off();
		vTaskDelay(1950 / portTICK_RATE_MS);
	}
}
