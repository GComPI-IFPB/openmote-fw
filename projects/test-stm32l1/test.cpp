/*
 * Copyright 2013 OpenMote Technologies, S.L.
 */

/**
 *
 * @file       test-stm32l1.cpp
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

#include "openmote-stm32l1.h"

/*================================ define ===================================*/

#define RED_LED_TASK_PRIORITY           ( tskIDLE_PRIORITY + 1 )

/*================================ typedef ==================================*/

/*=============================== variables =================================*/

GPIO_InitTypeDef GPIO_InitStructure;

/*=============================== prototypes ================================*/

static void prvRedLedTask(void *pvParameters);

/*================================= public ==================================*/

static void prvRedLedTask( void *pvParameters ) {
	while(true) {
		GPIO_ToggleBits(LED_RED_PORT, LED_RED_PIN);
		vTaskDelay(1000 / portTICK_RATE_MS);
	}
}

int main(void)
{
    RCC_AHBPeriphClockCmd(LED_GREEN_PERIPH, ENABLE);
    GPIO_InitStructure.GPIO_Pin = LED_GREEN_PIN;
    GPIO_InitStructure.GPIO_Mode = LED_GREEN_MODE;
    GPIO_InitStructure.GPIO_OType = LED_GREEN_TYPE;
    GPIO_InitStructure.GPIO_Speed = LED_GREEN_SPEED;
    GPIO_InitStructure.GPIO_PuPd = LED_GREEN_PUSH;
    GPIO_Init(LED_GREEN_PORT, &GPIO_InitStructure);
    
    RCC_AHBPeriphClockCmd(LED_RED_PERIPH, ENABLE);
    GPIO_InitStructure.GPIO_Pin = LED_RED_PIN;
    GPIO_InitStructure.GPIO_Mode = LED_RED_MODE;
    GPIO_InitStructure.GPIO_OType = LED_RED_TYPE;
    GPIO_InitStructure.GPIO_Speed = LED_RED_SPEED;
    GPIO_InitStructure.GPIO_PuPd = LED_RED_PUSH;
    GPIO_Init(LED_RED_PORT, &GPIO_InitStructure);
    
	xTaskCreate(prvRedLedTask, ( const char * ) "Red", 128, NULL, RED_LED_TASK_PRIORITY, NULL );

	vTaskStartScheduler();

    return 0;
}

/*================================ private ==================================*/

