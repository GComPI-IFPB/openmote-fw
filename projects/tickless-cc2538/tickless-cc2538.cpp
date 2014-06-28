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

#define greenLedTask_PRIORITY               ( tskIDLE_PRIORITY + 1 )

/*================================ typedef ==================================*/

/*=============================== variables =================================*/

Board board;

Led led_green(LED_GREEN_PORT, LED_GREEN_PIN);
Led led_orange(LED_ORANGE_PORT, LED_ORANGE_PIN);
Led led_red(LED_RED_PORT, LED_RED_PIN);
Led led_yellow(LED_YELLOW_PORT, LED_YELLOW_PIN);

Led debug_tick(GPIO_DEBUG_AD0_PORT, GPIO_DEBUG_AD0_PIN);
Led debug_idle(GPIO_DEBUG_AD1_PORT, GPIO_DEBUG_AD1_PIN);
Led debug_red(GPIO_DEBUG_AD2_PORT, GPIO_DEBUG_AD2_PIN);

Led tps62730(TPS62730_PORT, TPS62730_ONBYP_PIN);

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
    tps62730.off();
    
    xTaskCreate(prvGreenLedTask, ( const char * ) "Red", 128, NULL, greenLedTask_PRIORITY, NULL );      

	vTaskStartScheduler();
}

/*================================ private ==================================*/

extern "C" {

    void vApplicationTickHook(void) {
        debug_tick.toggle();
    }

    void vApplicationIdleHook(void) {
        debug_idle.toggle();
    }
}
