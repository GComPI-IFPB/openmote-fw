/*
 * Copyright 2013 OpenMote Technologies, S.L.
 */

/**
 *
 * @file       test-leds.cpp
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

#define GREEN_LED_TASK_PRIORITY             ( tskIDLE_PRIORITY + 1 )
#define RED_LED_TASK_PRIORITY               ( tskIDLE_PRIORITY + 0 )

/*================================ typedef ==================================*/

/*=============================== variables =================================*/

/*=============================== prototypes ================================*/

static void prvGreenLedTask(void *pvParameters);
static void prvRedLedTask(void *pvParameters);

/*================================= public ==================================*/

int main (void)
{
    // Set the TPS62730 in bypass mode (Vin = 3.3V, Iq < 1 uA)
    tps62730.setBypass();

    // Create two FreeRTOS tasks
    xTaskCreate(prvGreenLedTask, (const char *) "Green", 128, NULL, GREEN_LED_TASK_PRIORITY, NULL);
    xTaskCreate(prvRedLedTask, (const char *) "Red", 128, NULL, RED_LED_TASK_PRIORITY, NULL);

    // Kick the FreeRTOS scheduler
    vTaskStartScheduler();
}

static void prvRedLedTask(void *pvParameters)
{
    while(true)
    {
        vTaskDelay(100 / portTICK_RATE_MS);
        led_red.toggle();
    }
}

static void prvGreenLedTask(void *pvParameters)
{
    while(true)
    {
        led_green.off();
        vTaskDelay(950 / portTICK_RATE_MS);
        led_green.on();
        vTaskDelay(50 / portTICK_RATE_MS);
    }
}

/*================================ private ==================================*/

