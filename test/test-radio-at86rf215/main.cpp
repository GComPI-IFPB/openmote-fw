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

static void rf09Callback(void);
static void rf24Callback(void);

/*=============================== variables =================================*/

static PlainCallback rf09Callback_(rf09Callback);
static PlainCallback rf24Callback_(rf24Callback);

/*================================= public ==================================*/

int main(void)
{
    bool status;

	// Initialize board
	board.init();

    // Enable the SPI peripheral
    spi.enable(SPI_BAUDRATE);

    // Set radio callbacks
    at86rf215.setCallbacks(&rf09Callback_, &rf24Callback_);
    at86rf215.enableInterrupts();

    // Enable general interrupts
    board.enableInterrupts();

    // Forever and ever!
    while (true) {
        
        at86rf215.enable();

        board.delayMilliseconds(1000);
        
        status = at86rf215.check();

        if (status) {
            led_green.on();
        } else {
            led_red.on();
        }

        board.delayMilliseconds(1000);

        at86rf215.sleep();

        led_green.off();
        led_orange.off();
        led_yellow.off();
        led_red.off();

        board.delayMilliseconds(1000);
    }
    	
}

/*================================ private ==================================*/

static void rf09Callback(void)
{
    led_yellow.on();
}

static void rf24Callback(void)
{
    led_orange.on();
}