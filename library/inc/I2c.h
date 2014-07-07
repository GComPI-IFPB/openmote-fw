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

#include <stdint.h>

#include "Gpio.h"

#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"

class Gpio;

typedef void (*callback_t)(void);

class I2c
{

friend class InterruptHandler;

public:
    I2c(uint32_t peripheral_, Gpio* scl_, Gpio* sda_);
    void enable(uint32_t clock_);
    void sleep(void);
    void wakeup(void);
    void lock(void);
    void unlock(void);
    uint8_t readByte(uint8_t address_, uint8_t register_);
    uint8_t readByte(uint8_t address_, uint8_t register_, uint8_t * buffer, uint8_t size);
    void writeByte(uint8_t address_, uint8_t register_);
    void writeByte(uint8_t address_, uint8_t register_, uint8_t data_);
    void writeByte(uint8_t address_, uint8_t * data_, uint8_t size);;
protected:
    void interruptHandler(void);
private:
    uint32_t peripheral;
    uint32_t clock;
    Gpio* scl;
    Gpio* sda;
    SemaphoreHandle_t xMutex;
};

#endif /* I2C_H_ */
