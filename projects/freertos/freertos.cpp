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

#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"

#include "openmote-cc2538.h"

/*================================ define ===================================*/

#define GREEN_LED_TASK_PRIORITY             ( tskIDLE_PRIORITY + 0 )
#define BUTTON_TASK_PRIORITY                ( tskIDLE_PRIORITY + 1 )

/*================================ typedef ==================================*/

/*=============================== variables =================================*/

SemaphoreHandle_t xSemaphore = NULL;

/*=============================== prototypes ================================*/

static void prvGreenLedTask(void *pvParameters);
static void prvButtonTask(void *pvParameters);

static void button_user_callback(void);

/*================================= public ==================================*/

int main (void) {
    // Set the TPS62730 in bypass mode (Vin = 3.3V, Iq < 1 uA)
    tps62730_bypass.off();

    // Create two FreeRTOS tasks
	xTaskCreate(prvGreenLedTask, (const char *) "Green", 128, NULL, GREEN_LED_TASK_PRIORITY, NULL);
	xTaskCreate(prvButtonTask, (const char *) "Button", 128, NULL, BUTTON_TASK_PRIORITY, NULL);

    // Kick the FreeRTOS scheduler
	vTaskStartScheduler();
}

/*================================ private ==================================*/

static void button_user_callback(void)
{
    static BaseType_t xHigherPriorityTaskWoken; 
    xHigherPriorityTaskWoken = pdFALSE;
    xSemaphoreGiveFromISR(xSemaphore, &xHigherPriorityTaskWoken);
    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}

static void prvButtonTask( void *pvParameters ) {
    xSemaphore = xSemaphoreCreateMutex();

    button_user.setCallback(button_user_callback);
    button_user.enableInterrupt();

    while(true)
    {
        if (xSemaphoreTake( xSemaphore, ( TickType_t ) 10 ) == pdTRUE)
        {
            led_red.toggle();
        }
    } 
}

static void prvGreenLedTask( void *pvParameters )
{
	while(true)
	{
		led_green.toggle();
		vTaskDelay(1000 / portTICK_RATE_MS);
	}
}
