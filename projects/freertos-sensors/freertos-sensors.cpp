/*
 * Copyright 2013 OpenMote Technologies, S.L.
 */

/**
 *
 * @file       freertos.c
 * @author     Pere Tuset-Peiro (peretuset@openmote.com)
 * @version    v0.1
 * @date       May, 2014
 * @brief
 * @ingroup
 *
 */

/*================================ include ==================================*/

#include "Board.h"
#include "Button.h"
#include "Led.h"
#include "I2c.h"
#include "Uart.h"

#include "Adxl346.h"
#include "Max44009.h"
#include "Sht21.h"

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

Board board;

Led led_green(LED_GREEN_PORT, LED_GREEN_PIN);
Led led_orange(LED_ORANGE_PORT, LED_ORANGE_PIN);
Led led_red(LED_RED_PORT, LED_RED_PIN);
Led led_yellow(LED_YELLOW_PORT, LED_YELLOW_PIN);

Button button_user(BUTTON_USER_PORT, BUTTON_USER_PIN, BUTTON_USER_EDGE);

Gpio uart_rx(UART_RX_PORT, UART_RX_PIN);
Gpio uart_tx(UART_TX_PORT, UART_TX_PIN);
Uart uart(UART_PERIPHERAL, UART_PORT, UART_CLOCK, UART_INTERRUPT);

Gpio i2c_scl(I2C_PORT, I2C_SCL);
Gpio i2c_sda(I2C_PORT, I2C_SDA);
I2c i2c(I2C_PERIPHERAL, &i2c_scl, &i2c_sda);

Adxl346 adxl346(&i2c);
Max44009 max44009(&i2c);
Sht21 sht21(&i2c);

xSemaphoreHandle xSemaphore;

/*=============================== prototypes ================================*/

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
            vTaskDelay(1000 / portTICK_RATE_MS);
	    }
	} else {
	    led_orange.on();
	    vTaskDelete( NULL );
	}
}

static void prvLightTask( void *pvParameters ) {
    uint16_t light;
    
    vTaskDelay(500 / portTICK_RATE_MS);
    
    if (max44009.isPresent() == true) {
    	while(true) {
        	led_yellow.on();
        	max44009.readLux();
        	light = max44009.getLuxRaw();
           	led_yellow.off();
            vTaskDelay(1000 / portTICK_RATE_MS);
	    }
	} else {
	    led_yellow.on();
	    vTaskDelete( NULL );
	}
}

static void prvGreenLedTask( void *pvParameters ) {
	while(true) {
		led_green.on();
		vTaskDelay(10 / portTICK_RATE_MS);
        led_green.off();
		vTaskDelay(990 / portTICK_RATE_MS);
	}
}

int main (void) {
    button_user.setCallback(button_user_callback);
    button_user.enableInterrupt();
    
    uart.setRxGpio(uart_rx, UART_RX_IOC);
    uart.setTxGpio(uart_tx, UART_TX_IOC);
    uart.init(UART_BAUDRATE, UART_CONFIG, UART_TX_INT_MODE);
    
    i2c.init(100000);
    
    xSemaphore = xSemaphoreCreateMutex();
    
    xTaskCreate(prvGreenLedTask, ( const char * ) "GreenLed", 128, NULL, GREEN_LED_TASK_PRIORITY, NULL );
    xTaskCreate(prvTemperatureTask, ( const char * ) "Temperature", 128, NULL, TEMPERATURE_TASK_PRIORITY, NULL );
    xTaskCreate(prvLightTask, ( const char * ) "Light", 128, NULL, LIGHT_TASK_PRIORITY, NULL );
    xTaskCreate(prvButtonTask, ( const char * ) "Button", 128, NULL, BUTTON_TASK_PRIORITY, NULL );

    vTaskStartScheduler();
}

/*================================ private ==================================*/

