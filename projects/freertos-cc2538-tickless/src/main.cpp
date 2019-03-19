/**
 * @file       main.cpp
 * @author     Pere Tuset-Peiro (peretuset@openmote.com)
 * @version    v0.1
 * @date       November, 2018
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

#define GREEN_LED_TASK_NAME                 ( "LedTask" )
#define GREEN_LED_STACK_SIZE                ( 128 )
#define GREEN_LED_TASK_PRIORITY             ( tskIDLE_PRIORITY + 0 )

#define BUTTON_TASK_NAME                    ( "ButtonTask" )
#define BUTTON_STACK_SIZE                   ( 128 )
#define BUTTON_TASK_PRIORITY                ( tskIDLE_PRIORITY + 1 )

/*================================ typedef ==================================*/

/*=============================== prototypes ================================*/

extern "C" TickType_t board_sleep(TickType_t xModifiableIdleTime);
extern "C" TickType_t board_wakeup(TickType_t xModifiableIdleTime);

static void prvGreenLedTask(void *pvParameters);
static void prvButtonTask(void *pvParameters);

static void buttonCallback(void);

/*=============================== variables =================================*/

static SemaphoreBinary buttonSemaphore(false);
static PlainCallback userCallback(buttonCallback);

static Task task1(GREEN_LED_TASK_NAME, GREEN_LED_STACK_SIZE, GREEN_LED_TASK_PRIORITY, prvGreenLedTask, nullptr);
static Task task2(BUTTON_TASK_NAME, BUTTON_STACK_SIZE, BUTTON_TASK_PRIORITY, prvButtonTask, nullptr);

/*================================= public ==================================*/

int main(void)
{ 
  /* Initialize the board */
  board.init();
  
  /* Start the scheduler */
  Scheduler::run();
}

TickType_t board_sleep(TickType_t xModifiableIdleTime)
{
  /* FreeRTOS callback before sleep */
  return xModifiableIdleTime;
}

TickType_t board_wakeup(TickType_t xModifiableIdleTime)
{
  /* FreeRTOS callback after sleep */
  return xModifiableIdleTime;
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
    /* Turn on green LED for 10 ms */
    led_green.on();
    Scheduler::delay_ms(10);
    
    /* Turn off green LED for 990 ms */
    led_green.off();
    Scheduler::delay_ms(990);
  }
}
