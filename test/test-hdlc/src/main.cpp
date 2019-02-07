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

#include "BoardImplementation.hpp"

#include "Callback.hpp"
#include "Scheduler.hpp"
#include "Semaphore.hpp"
#include "Task.hpp"

/*================================ define ===================================*/

#define GREEN_LED_TASK_PRIORITY             ( tskIDLE_PRIORITY + 0 )
#define HDLC_TASK_PRIORITY                  ( tskIDLE_PRIORITY + 1 )

/*================================ typedef ==================================*/

/*=============================== prototypes ================================*/

static void prvGreenLedTask(void *pvParameters);
static void prvSerialTask(void *pvParameters);

/*=============================== variables =================================*/

static Task heartbeatTask{(const char *) "Green", 128, GREEN_LED_TASK_PRIORITY, prvGreenLedTask, nullptr};
static Task hdlcTask{(const char *) "Hdlc", 128, HDLC_TASK_PRIORITY, prvHdlcTask, nullptr};

/*================================= public ==================================*/

int main (void)
{
  /* Initialize board */
  board.init();

  /* Start the scheduler */
  Scheduler::run();
}

/*=============================== protected =================================*/

/*================================ private ==================================*/

static void prvHdlcTask(void *pvParameters)
{
  /* Forever */
  while (true)
  {
  }
}

static void prvGreenLedTask(void *pvParameters)
{
  /* Forever */
  while (true)
  {
    /* Turn off green LED for 990 ms */
    led_green.off();
    Scheduler::delay_ms(990);

    /* Turn on green LED for 10 ms */
    led_green.on();
    Scheduler::delay_ms(10);
  }
}
