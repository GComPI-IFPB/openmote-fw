/*
 * Copyright 2013 OpenMote Technologies, S.L.
 */

/**
 *
 * @file       tickless-cc2538.cpp
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

extern "C" TickType_t board_sleep(TickType_t xModifiableIdleTime);
extern "C" TickType_t board_wakeup(TickType_t xModifiableIdleTime);

static void prvGreenLedTask(void *pvParameters);

/*================================= public ==================================*/

int main (void) {
    // Set the TPS62730 in bypass mode (Vin = 3.3V, Iq < 1 uA)
    tps62730_bypass.off();
    
    // Create a task that blinks the green led once every 5 seconds
    xTaskCreate(prvGreenLedTask, ( const char * ) "Red", 128, NULL, greenLedTask_PRIORITY, NULL );      
    
    // Kick the FreeRTOS scheduler
	vTaskStartScheduler();
}

TickType_t board_sleep(TickType_t xModifiableIdleTime)
{
    return xModifiableIdleTime;
}

TickType_t board_wakeup(TickType_t xModifiableIdleTime)
{
    return xModifiableIdleTime;
}

/*================================ private ==================================*/

static void prvGreenLedTask( void *pvParameters ) {
    while(true) {
        led_green.off();
        vTaskDelay(4950 / portTICK_PERIOD_MS);
        led_green.on();
        vTaskDelay(50 / portTICK_PERIOD_MS);
	}
}

