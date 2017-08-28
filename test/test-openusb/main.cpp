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

#include "Scheduler.h"
#include "Task.h"

/*================================ define ===================================*/

/*================================ typedef ==================================*/

/*=============================== prototypes ================================*/

/*=============================== variables =================================*/

/*================================= public ==================================*/

int main(void) {
	// Initialize the board
    board.init();

    // Start the scheduler
    Scheduler::run();
}

/*================================ private ==================================*/
