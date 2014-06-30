/*
 * Copyright 2013 OpenMote Technologies, S.L.
 */

/**
 *
 * @file       I2c.h
 * @author     Pere Tuset-Peiro (peretuset@openmote.com)
 * @version    v0.1
 * @date       May, 2014
 * @brief
 * @ingroup
 *
 */

#ifndef I2C_H_
#define I2C_H_

#include "interrupt.h"
#include "ioc.h"
#include "i2c.h"
#include "sys_ctrl.h"

#include "hw_ioc.h"
#include "hw_ints.h"
#include "hw_types.h"
#include "hw_i2cm.h"
#include "hw_i2cs.h"

#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"

class Gpio;

class I2c
{
public:
    I2c(uint32_t peripheral_, Gpio* scl_, Gpio* sda_);
    void enable(uint32_t clock_);
    void sleep(void);
    void wakeup(void);
    uint8_t readByte(uint8_t address_, uint8_t register_);
    uint8_t readByte(uint8_t address_, uint8_t register_, uint8_t * buffer, uint8_t size);
    void writeByte(uint8_t address_, uint8_t register_);
    void writeByte(uint8_t address_, uint8_t register_, uint8_t data_);
    void writeByte(uint8_t address_, uint8_t * data_, uint8_t size);
    void lock(void);
    void unlock(void);
    void interruptEnable(void);
    void interruptDisable(void);
    void interruptHandler(void);
private:
    uint32_t peripheral;
    uint32_t clock;
    Gpio* scl;
    Gpio* sda;
    SemaphoreHandle_t xMutex;
};

#endif /* I2C_H_ */
