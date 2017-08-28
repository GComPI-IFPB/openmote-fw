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

#include "At86rf215.h"
#include "At86rf215_regs.h"

#include "Callback.h"
#include "Scheduler.h"
#include "Semaphore.h"
#include "Task.h"

/*================================ define ===================================*/

#define SPI_BAUDRATE                        ( 8000000 )

/*================================ typedef ==================================*/

/*=============================== prototypes ================================*/

/*=============================== variables =================================*/

/*================================= public ==================================*/

int main(void)
{
	// Initialize board
	board.init();

    // Enable the SPI peripheral
    spi.enable(SPI_BAUDRATE);

    // Put CC1200 in transmit mode
    at86rf215.on();

	if(at86rf215.check()) {
        led_green.on();
    } else {
        led_red.on();
    }

    // Forever and ever!
    while(true)
    	;
}

/*================================ private ==================================*/
