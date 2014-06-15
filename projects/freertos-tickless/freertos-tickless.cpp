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

#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"

#include "openmote-cc2538.h"

/*================================ define ===================================*/

#define mainCHECK_TASK_PRIORITY				( tskIDLE_PRIORITY + 1 )

/*================================ typedef ==================================*/

/*=============================== variables =================================*/

Board board;

Led led_green(LED_GREEN_PORT, LED_GREEN_PIN);
Led led_orange(LED_ORANGE_PORT, LED_ORANGE_PIN);
Led led_red(LED_RED_PORT, LED_RED_PIN);
Led led_yellow(LED_YELLOW_PORT, LED_YELLOW_PIN);

Led debug_tick(GPIO_DEBUG_PORT, GPIO_DEBUG_AD0);
Led debug_idle(GPIO_DEBUG_PORT, GPIO_DEBUG_AD1);
Led debug_task(GPIO_DEBUG_PORT, GPIO_DEBUG_AD2);
Led debug_green(GPIO_DEBUG_PORT, GPIO_DEBUG_AD3);

Button button_user(BUTTON_USER_PORT, BUTTON_USER_PIN, BUTTON_USER_EDGE);

SemaphoreHandle_t xSemaphore = NULL;

/*=============================== prototypes ================================*/

static void prvGreenLedTask(void *pvParameters);
static void prvRedLedTask(void *pvParameters);
static void button_user_callback(void);

/*================================= public ==================================*/

static void button_user_callback(void) {
    static BaseType_t xHigherPriorityTaskWoken; 
    xHigherPriorityTaskWoken = pdFALSE;
    xSemaphoreGiveFromISR( xSemaphore, &xHigherPriorityTaskWoken );
    portYIELD_FROM_ISR( xHigherPriorityTaskWoken );
}

static void prvRedLedTask( void *pvParameters ) {
    while(true) {
        if (xSemaphoreTake( xSemaphore, portMAX_DELAY ) == pdTRUE) {
            debug_task.toggle();
            led_red.toggle();
        }
	}
}

static void prvGreenLedTask( void *pvParameters ) {
    while(true) {
        led_green.toggle();
        debug_green.toggle();
        vTaskDelay(1000 / portTICK_PERIOD_MS);
	}
}

int main (void) {    
    button_user.setCallback(button_user_callback);
    button_user.enableInterrupt();
    
    xSemaphore = xSemaphoreCreateMutex();

    xTaskCreate(prvGreenLedTask, ( const char * ) "Green", 128, NULL, mainCHECK_TASK_PRIORITY, NULL );      
	xTaskCreate(prvRedLedTask, ( const char * ) "Red", 128, NULL, mainCHECK_TASK_PRIORITY, NULL );

	vTaskStartScheduler();
}

/*================================ private ==================================*/

extern "C" {

    void SleepTimerHandler(void) {
    }

    void vApplicationTickHook(void) {
        debug_tick.toggle();
    }

    void vApplicationIdleHook(void) {
        debug_idle.toggle();
    }

}
