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

#include "BoardImplementation.hpp"

#include "Gpio.hpp"

#include "Callback.hpp"
#include "Scheduler.hpp"
#include "Semaphore.hpp"
#include "Task.hpp"

/*================================ define ===================================*/

#define GREEN_LED_TASK_PRIORITY             ( tskIDLE_PRIORITY + 0 )
#define BUTTON_TASK_PRIORITY                ( tskIDLE_PRIORITY + 1 )

/*================================ typedef ==================================*/

/*=============================== prototypes ================================*/

static void prvGreenLedTask(void *pvParameters);
static void prvButtonTask(void *pvParameters);

static void buttonCallback(void);

/*=============================== variables =================================*/

static SemaphoreBinary buttonSemaphore;

static PlainCallback userCallback {buttonCallback};

static Task heartbeatTask {(const char *) "Green", 128, GREEN_LED_TASK_PRIORITY, prvGreenLedTask, nullptr};
static Task buttonTask {(const char *) "Button", 128, BUTTON_TASK_PRIORITY, prvButtonTask, nullptr};

/*================================= public ==================================*/

int main(void)
{
  /* Initialize the board */
  board.init();

  /* Start the scheduler */
  Scheduler::run();
}

/*================================ private ==================================*/

static void buttonCallback(void)
{
	/* Give the buttonSemaphore from the interrupt */
  buttonSemaphore.giveFromInterrupt();
}

static void prvButtonTask(void *pvParameters)
{
  /* Configure the user button */
  button_user.setCallback(&userCallback);
  button_user.enableInterrupts();

  buttonSemaphore.take();

  /* Forever */
  while (true)
  {
    /* Take the buttonSemaphore, block until available */
    if (buttonSemaphore.take())
    {
        /* Toggle the red LED */
        led_red.toggle();
    }
  }
}

static void prvGreenLedTask(void *pvParameters)
{
  /* Forever */
  while (true)
  {
    /* Turn on green LED for 100 ms */
    led_green.on();
    Scheduler::delay_ms(100);
    
    /* Turn off green LED for 900 ms */
    led_green.off();
    Scheduler::delay_ms(900);
  }
}
