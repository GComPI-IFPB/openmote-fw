/**
 * @file       main.cpp
 * @author     Pere Tuset-Peiro (peretuset@openmote.com)
 * @version    v0.1
 * @date       May, 2015
 * @brief
 *
 * @copyright  Copyright 2015, OpenMote Technologies, S.L.
 *             This file is licensed under the GNU General Public License v2.
 */

/*================================ include ==================================*/

#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"

#include "openmote-cc2538.h"

#include "Board.h"
#include "Gpio.h"

#include "Tps62730.h"

#include "Scheduler.h"
#include "Task.h"

/*================================ define ===================================*/

#define GREEN_LED_TASK_PRIORITY             ( tskIDLE_PRIORITY + 1 )
#define RED_LED_TASK_PRIORITY               ( tskIDLE_PRIORITY + 0 )

/*================================ typedef ==================================*/

/*=============================== prototypes ================================*/

static void prvGreenLedTask(void *pvParameters);
static void prvRedLedTask(void *pvParameters);

/*=============================== variables =================================*/

/*================================= public ==================================*/

int main (void)
{
    // Set the TPS62730 in bypass mode (Vin = 3.3V, Iq < 1 uA)
    tps62730.setBypass();

    // Create two FreeRTOS tasks
    xTaskCreate(prvGreenLedTask, (const char *) "Green", 128, NULL, GREEN_LED_TASK_PRIORITY, NULL);
    xTaskCreate(prvRedLedTask, (const char *) "Red", 128, NULL, RED_LED_TASK_PRIORITY, NULL);

    // Start the scheduler
    Scheduler::run();
}

static void prvRedLedTask(void *pvParameters)
{
    // Forever
    while(true)
    {
        // Togle the red LED every 100 ms
        Task::delay(100);
        led_red.toggle();
    }
}

static void prvGreenLedTask(void *pvParameters)
{
    // Forever
    while(true)
    {
        // Turn off green LED for 950 ms
        led_green.off();
        Task::delay(950);
        
        // Turn on green LED for 50 ms
        led_green.on();
        Task::delay(50);
    }
}

/*================================ private ==================================*/

