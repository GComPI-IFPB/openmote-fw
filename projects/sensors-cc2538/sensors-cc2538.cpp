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

static xSemaphoreHandle xSemaphore;

/*=============================== prototypes ================================*/

extern "C" void board_sleep(void);
extern "C" void board_wakeup(void);

static void prvGreenLedTask(void *pvParameters);
static void prvLightTask(void *pvParameters);
static void prvTemperatureTask(void *pvParameters);
static void prvButtonTask(void *pvParameters);
static void button_user_callback(void);

/*================================= public ==================================*/

static void button_user_callback(void) {
    static BaseType_t xHigherPriorityTaskWoken; 
    xHigherPriorityTaskWoken = pdFALSE;
    xSemaphoreGiveFromISR( xSemaphore, &xHigherPriorityTaskWoken );
    portYIELD_FROM_ISR( xHigherPriorityTaskWoken );
}

static void prvButtonTask( void *pvParameters ) {
    while(true) {
        if (xSemaphoreTake( xSemaphore, ( TickType_t ) 10 ) == pdTRUE) {
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

int main (void) {
    tps62730.off();

    button_user.setCallback(button_user_callback);
    button_user.enableInterrupt();
    
    // uart.setRxGpio(uart_rx, UART_RX_IOC);
    // uart.setTxGpio(uart_tx, UART_TX_IOC);
    // uart.init(UART_BAUDRATE, UART_CONFIG, UART_TX_INT_MODE);
    
    i2c.enable(100000);
    
    xSemaphore = xSemaphoreCreateMutex();
    
    xTaskCreate(prvGreenLedTask, ( const char * ) "GreenLed", 128, NULL, GREEN_LED_TASK_PRIORITY, NULL );
    xTaskCreate(prvTemperatureTask, ( const char * ) "Temperature", 128, NULL, TEMPERATURE_TASK_PRIORITY, NULL );
    xTaskCreate(prvLightTask, ( const char * ) "Light", 128, NULL, LIGHT_TASK_PRIORITY, NULL );
    xTaskCreate(prvButtonTask, ( const char * ) "Button", 128, NULL, BUTTON_TASK_PRIORITY, NULL );

    vTaskStartScheduler();
}

void board_sleep(void)
{
    i2c.sleep();
}

void board_wakeup(void)
{
    i2c.wakeup();
}

/*================================ private ==================================*/

