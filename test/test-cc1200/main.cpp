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

#include "openmote-cc2538.h"

#include "Board.h"
#include "Gpio.h"
#include "Spi.h"
#include "Tps62730.h"

#include "Cc1200.h"
#include "Cc1200_regs.h"

#include "Callback.h"
#include "Scheduler.h"
#include "Semaphore.h"
#include "Task.h"

/*================================ define ===================================*/

#define SPI_BAUDRATE                        ( 4000000 )

/*================================ typedef ==================================*/

/*=============================== prototypes ================================*/

/*=============================== variables =================================*/

/*================================= public ==================================*/

int main(void)
{
    // Set the TPS62730 in bypass mode (Vin = 3.3V, Iq < 1 uA)
    tps62730.setBypass();

    // Enable the SPI peripheral
    spi.enable(SPI_BAUDRATE);

    // Put CC1200 in transmit mode
    cc1200.transmit();

    // Forever and ever!
    while(true)
    	;
}

/*================================ private ==================================*/
