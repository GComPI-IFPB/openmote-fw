/*
 * Copyright 2013 OpenMote Technologies, S.L.
 */

/**
 *
 * @file       freertos.cpp
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

#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"

#include "openmote-cc2538.h"

/*================================ define ===================================*/

#define RED_LED_TASK_PRIORITY               ( tskIDLE_PRIORITY + 1 )
#define BUTTON_TASK_PRIORITY                ( tskIDLE_PRIORITY + 0 )

/*================================ typedef ==================================*/

/*=============================== variables =================================*/

Board board;

Led led_green(LED_GREEN_PORT, LED_GREEN_PIN);
Led led_orange(LED_ORANGE_PORT, LED_ORANGE_PIN);
Led led_red(LED_RED_PORT, LED_RED_PIN);
Led led_yellow(LED_YELLOW_PORT, LED_YELLOW_PIN);

Button button_user(BUTTON_USER_PORT, BUTTON_USER_PIN, BUTTON_USER_EDGE);

SemaphoreHandle_t xSemaphore = NULL;

/*=============================== prototypes ================================*/

static void prvRedLedTask(void *pvParameters);
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
            led_orange.toggle();
        }
    } 
}

static void prvRedLedTask( void *pvParameters ) {
	while(true) {
		led_red.toggle();
		vTaskDelay(1000 / portTICK_RATE_MS);
	}
}

int main (void) {
    led_green.on();
    
    button_user.setCallback(button_user_callback);
    button_user.enableInterrupt();
    
    xSemaphore = xSemaphoreCreateMutex();
    
	xTaskCreate(prvRedLedTask, ( const char * ) "Red", 128, NULL, RED_LED_TASK_PRIORITY, NULL );
	xTaskCreate(prvButtonTask, ( const char * ) "Button", 128, NULL, BUTTON_TASK_PRIORITY, NULL);

	vTaskStartScheduler();
}

/*================================ private ==================================*/

