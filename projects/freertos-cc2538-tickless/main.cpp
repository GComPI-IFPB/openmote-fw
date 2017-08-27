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
#include "platform_types.h"

#include "Board.h"
#include "Gpio.h"
#include "Spi.h"

#include "Cc1200.h"
#include "Cc1200_regs.h"

#include "Callback.h"
#include "Scheduler.h"
#include "Semaphore.h"
#include "Task.h"

/*================================ define ===================================*/

#define GREEN_LED_TASK_PRIORITY             ( tskIDLE_PRIORITY + 0 )
#define BUTTON_TASK_PRIORITY                ( tskIDLE_PRIORITY + 1 )

#define SPI_BAUDRATE                        ( 4000000 )

/*================================ typedef ==================================*/

/*=============================== prototypes ================================*/

extern "C" TickType_t board_sleep(TickType_t xModifiableIdleTime);
extern "C" TickType_t board_wakeup(TickType_t xModifiableIdleTime);

static void prvGreenLedTask(void *pvParameters);
static void prvButtonTask(void *pvParameters);

static void buttonCallback(void);

/*=============================== variables =================================*/

static SemaphoreBinary buttonSemaphore;

static PlainCallback userCallback(buttonCallback);

/*================================= public ==================================*/

int main(void)
{
    // Initialize the board
    board.init();

    // Create two FreeRTOS tasks
    xTaskCreate(prvGreenLedTask, (const char *) "Green", 128, NULL, GREEN_LED_TASK_PRIORITY, NULL);

    // Enable the SPI peripheral
    spi.enable(SPI_BAUDRATE);

    // Put CC1200 in transmit mode
    cc1200.sleep();

    // Start the scheduler
    Scheduler::run();
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

static void buttonCallback(void)
{  
    // Give the buttonSemaphore from the interrupt
    buttonSemaphore.giveFromInterrupt();
}

static void prvButtonTask(void *pvParameters)
{
    // Set the button callback and enable interrupts
    button_user.setCallback(&userCallback);
    button_user.enableInterrupts();

    buttonSemaphore.take();

    // Forever
    while (true) {
        // Take the buttonSemaphore, block until available
        if (buttonSemaphore.take()) {
            // Toggle the red LED
            led_red.toggle();
        }
    }
}

static void prvGreenLedTask(void *pvParameters)
{
    // Forever
    while (true) {
        // Turn off green LED for 1950 ms
        led_green.off();
         vTaskDelay(1950 / portTICK_RATE_MS);

        // Turn on green LED for 50 ms
        led_green.on();
        vTaskDelay(50 / portTICK_RATE_MS);
    }
}
