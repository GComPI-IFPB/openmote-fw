/*
 * Copyright 2013 OpenMote Technologies, S.L.
 */

/**
 *
 * @file       I2cDriver.cpp
 * @author     Pere Tuset-Peiro (peretuset@openmote.com)
 * @version    v0.1
 * @date       May, 2014
 * @brief
 * @ingroup
 *
 */

/*================================ include ==================================*/

#include "I2cDriver.h"

/*================================ define ===================================*/

/*================================ typedef ==================================*/

/*=============================== variables =================================*/

/*=============================== prototypes ================================*/

/*================================= public ==================================*/

I2cDriver::I2cDriver(uint32_t peripheral, GpioI2c& scl, GpioI2c& sda):
    I2c(peripheral, scl, sda)
{
    xMutex_ = xSemaphoreCreateMutex();
    if (xMutex_ == NULL) {
        while(true);
    }
}

void I2cDriver::lock(void)
{
    xSemaphoreTake(xMutex_, portMAX_DELAY);
}

void I2cDriver::unlock(void)
{
    xSemaphoreGive(xMutex_);
}

/*=============================== protected =================================*/

/*================================ private ==================================*/
