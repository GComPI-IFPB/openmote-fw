/*
 * Copyright 2013 OpenMote Technologies, S.L.
 */

/**
 *
 * @file       SpiDriver.cpp
 * @author     Pere Tuset-Peiro (peretuset@openmote.com)
 * @version    v0.1
 * @date       May, 2014
 * @brief
 * @ingroup
 *
 */

/*================================ include ==================================*/

#include "SpiDriver.h"

/*================================ define ===================================*/

/*================================ typedef ==================================*/

/*=============================== variables =================================*/

/*=============================== prototypes ================================*/

/*================================= public ==================================*/

SpiDriver::SpiDriver(uint32_t peripheral_, uint32_t base_, uint32_t clock_, \
         GpioSpi* miso_, GpioSpi* mosi_, GpioSpi* clk_, GpioSpi* ncs_):
    Spi(peripheral_, base_, clock_, miso_, mosi_, clk_, ncs_)
{
    xMutex = xSemaphoreCreateMutex();
    if (xMutex == NULL) {
        while(true);
    }
}

void SpiDriver::lock(void)
{
    xSemaphoreTake(xMutex, portMAX_DELAY);
}

void SpiDriver::unlock(void)
{
    xSemaphoreGive(xMutex);
}

/*=============================== protected =================================*/

/*================================ private ==================================*/
