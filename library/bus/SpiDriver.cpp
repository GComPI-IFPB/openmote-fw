/**
 * @file       SpiDriver.cpp
 * @author     Pere Tuset-Peiro (peretuset@openmote.com)
 * @version    v0.1
 * @date       May, 2015
 * @brief
 *
 * @copyright  Copyright 2015, OpenMote Technologies, S.L.
 *             This file is licensed under the GNU General Public License v2.
 */

/*================================ include ==================================*/

#include "SpiDriver.h"

/*================================ define ===================================*/

/*================================ typedef ==================================*/

/*=============================== variables =================================*/

/*=============================== prototypes ================================*/

/*================================= public ==================================*/

SpiDriver::SpiDriver(uint32_t peripheral, uint32_t base, uint32_t clock, \
         GpioSpi& miso, GpioSpi& mosi, GpioSpi& clk, GpioSpi& ncs):
    Spi(peripheral, base, clock, miso, mosi, clk, ncs)
{
    xMutex_ = xSemaphoreCreateMutex();
    if (xMutex_ == NULL) {
        while(true);
    }
}

void SpiDriver::lock(void)
{
    xSemaphoreTake(xMutex_, portMAX_DELAY);
}

void SpiDriver::unlock(void)
{
    xSemaphoreGive(xMutex_);
}

/*=============================== protected =================================*/

/*================================ private ==================================*/
