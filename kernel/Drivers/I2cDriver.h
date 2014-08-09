/*
 * Copyright 2013 OpenMote Technologies, S.L.
 */

/**
 *
 * @file       I2cDriver.h
 * @author     Pere Tuset-Peiro (peretuset@openmote.com)
 * @version    v0.1
 * @date       May, 2014
 * @brief
 * @ingroup
 *
 */

#ifndef I2C_DRIVER_H_
#define I2C_DRIVER_H_

#include "I2c.h"

#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"

class I2cDriver : public I2c
{

public:
    I2cDriver(uint32_t peripheral_, GpioI2c* scl_, GpioI2c* sda_);
    void lock(void);
    void unlock(void);
private:
    SemaphoreHandle_t xMutex;
};

#endif /* I2C_DRIVER_H_ */
