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

#include "Led.h"
#include "Board.h"

#include "FreeRTOS.h"
#include "task.h"

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

/*=============================== prototypes ================================*/

static void prvRedLedTask(void *pvParameters);

/*================================= public ==================================*/

static void prvRedLedTask( void *pvParameters ) {
	while(true) {
		led_orange.toggle();
		vTaskDelay(1000 / portTICK_RATE_MS);
		led_red.toggle();
	}
}

int main (void) {
    led_green.on();
    
	xTaskCreate(prvRedLedTask, ( const char * ) "Red", 128, NULL, mainCHECK_TASK_PRIORITY, NULL );

	vTaskStartScheduler();
}

/*================================ private ==================================*/

extern "C" {
    void SleepTimerHandler(void) {
    }
}

