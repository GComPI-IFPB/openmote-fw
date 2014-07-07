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

#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"

#include "openmote-cc2538.h"

/*================================ define ===================================*/

#define greenLedTask_PRIORITY               ( tskIDLE_PRIORITY + 1 )

/*================================ typedef ==================================*/

/*=============================== variables =================================*/

/*=============================== prototypes ================================*/

static void prvGreenLedTask(void *pvParameters);

/*================================= public ==================================*/

static void prvGreenLedTask( void *pvParameters ) {
    while(true) {
        led_green.off();
        vTaskDelay(4950 / portTICK_PERIOD_MS);
        led_green.on();
        vTaskDelay(50 / portTICK_PERIOD_MS);
	}
}

int main (void) {    
    tps62730_bypass.off();
    
    xTaskCreate(prvGreenLedTask, ( const char * ) "Red", 128, NULL, greenLedTask_PRIORITY, NULL );      

	vTaskStartScheduler();
}

/*================================ private ==================================*/

extern "C" {

    void vApplicationTickHook(void) {
        debug_ad0.toggle();
    }

    void vApplicationIdleHook(void) {
        debug_ad1.toggle();
    }
}
