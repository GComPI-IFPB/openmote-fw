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

#include "board.h"

#include "Rendezvouz.h"
#include "Scheduler.h"
#include "Semaphore.h"
#include "Task.h"

/*================================ define ===================================*/

#define GREEN_LED_TASK_PRIORITY             ( tskIDLE_PRIORITY + 0 )
#define RED_LED_TASK_PRIORITY               ( tskIDLE_PRIORITY + 1 )
#define ORANGE_LED_TASK_PRIORITY               ( tskIDLE_PRIORITY + 2 )

/*================================ typedef ==================================*/

/*=============================== prototypes ================================*/

static void prvGreenLedTask(void *pvParameters);
static void prvRedLedTask(void *pvParameters);
static void prvOrangeLedTask(void *pvParameters);

/*=============================== variables =================================*/

static Rendezvouz rendezvouz;

/*================================= public ==================================*/

int main (void)
{
    // Initialize board
    board.init();

    // Create two FreeRTOS tasks
    xTaskCreate(prvGreenLedTask, (const char *) "Green", 128, NULL, GREEN_LED_TASK_PRIORITY, NULL);
    xTaskCreate(prvRedLedTask, (const char *) "Red", 128, NULL, RED_LED_TASK_PRIORITY, NULL);
    xTaskCreate(prvOrangeLedTask, (const char *) "Orange", 128, NULL, ORANGE_LED_TASK_PRIORITY, NULL);

    // Start the scheduler
    Scheduler::run();
}

/*=============================== protected =================================*/

/*================================ private ==================================*/

static void prvGreenLedTask(void *pvParameters)
{
    static uint8_t taskId;

    taskId = rendezvouz.getTaskId();

    // Forever
    while (true) {
        // Turn off green LED for 1950 ms
        led_green.off();
        vTaskDelay(50 / portTICK_RATE_MS);

        // Synchronization point with maxium delay of 1000 ms
        rendezvouz.sync(taskId, 1000);

        // Turn on green LED for 50 ms
        led_green.on();
        vTaskDelay(50 / portTICK_RATE_MS);
    }
}

static void prvRedLedTask(void *pvParameters)
{
    static uint8_t taskId;

    taskId = rendezvouz.getTaskId();

    // Forever
    while (true) {
        // Turn off green LED for 1950 ms
        led_red.off();
        vTaskDelay(1950 / portTICK_RATE_MS);

        // Synchronization point with infinite delay
        rendezvouz.sync(taskId);

        // Turn on green LED for 50 ms
        led_red.on();
        vTaskDelay(50 / portTICK_RATE_MS);
    }
}

static void prvOrangeLedTask(void *pvParameters)
{
    static uint8_t taskId;

    taskId = rendezvouz.getTaskId();

    // Forever
    while (true) {
        // Turn off green LED for 1950 ms
        led_orange.off();
        vTaskDelay(50 / portTICK_RATE_MS);

        // Synchronization point with maximum delay of 100 ms
        rendezvouz.sync(taskId, 100);

        // Turn on green LED for 50 ms
        led_orange.on();
        vTaskDelay(50 / portTICK_RATE_MS);
    }
}