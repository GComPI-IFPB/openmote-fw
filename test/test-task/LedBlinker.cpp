/**
 * @file       LedBlinker.cpp
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

/*================================ define ===================================*/

/*================================ typedef ==================================*/

/*=============================== prototypes ================================*/

/*=============================== variables =================================*/

/*================================= public ==================================*/

LedBlinker::LedBlinker(const char * const name, uint8_t priority, GpioOut& gpio):
	Task(name, priority), gpio_(gpio)
{
}

void LedBlinker::setTime(uint32_t timeOn, uint32_t period)
{
	timeOn_ = timeOn;
	period_ = period;
}

void LedBlinker::run(void) 
{
    // Forever
    while (true) {
    	// Turn LED on and wait
    	gpio_.on();
    	Task::delay(timeOn_);

		// Turn LED off and wait
    	gpio_.off();
    	Task::delay(period_ - timeOn_);
    }
}

/*================================ private ==================================*/
