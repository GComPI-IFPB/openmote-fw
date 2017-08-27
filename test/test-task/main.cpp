/**
 * @file       main.cpp
 * @author     Pere Tuset-Peiro (peretuset@openmote.com)
 * @version    v0.1
 * @date       May, 2016
 * @brief
 *
 * @copyright  Copyright 2016, OpenMote Technologies, S.L.
 *             This file is licensed under the GNU General Public License v2.
 */

/*================================ include ==================================*/

#include "board.h"

#include "LedBlinker.h"

#include "Scheduler.h"
#include "Task.h"

/*================================ define ===================================*/

#define RED_LED_TASK_PRIORITY             ( tskIDLE_PRIORITY + 0 )
#define GREEN_LED_TASK_PRIORITY           ( tskIDLE_PRIORITY + 1 )
#define YELLOW_LED_TASK_PRIORITY          ( tskIDLE_PRIORITY + 2 )
#define ORANGE_LED_TASK_PRIORITY          ( tskIDLE_PRIORITY + 3 )

/*================================ typedef ==================================*/

/*=============================== prototypes ================================*/

/*=============================== variables =================================*/

static LedBlinker redLedBlinker("redLedBlinker", RED_LED_TASK_PRIORITY, led_red);
static LedBlinker greenLedBlinker("greenLedBlinker", GREEN_LED_TASK_PRIORITY, led_green);
static LedBlinker yellowLedBlinker("yellowLedBlinker", YELLOW_LED_TASK_PRIORITY, led_yellow);
static LedBlinker orangeLedBlinker("orangeLedBlinker", ORANGE_LED_TASK_PRIORITY, led_orange);

/*================================= public ==================================*/

int main(void) {
	// Initialize the board
    board.init();

    // Set blinking periods
    greenLedBlinker.setTime(100, 1600);
    yellowLedBlinker.setTime(200, 1600);
    orangeLedBlinker.setTime(400, 1600);
    redLedBlinker.setTime(800, 1600);

    // Start the scheduler
    Scheduler::run();
}

/*================================ private ==================================*/
