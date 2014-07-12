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
    bool readByte(uint8_t address_, uint8_t * buffer);
    bool readByte(uint8_t address_, uint8_t * buffer, uint8_t size);
    bool writeByte(uint8_t address_, uint8_t register_);
    bool writeByte(uint8_t address_, uint8_t * buffer, uint8_t size);
protected:
    void interruptHandler(void);
protected:
    uint32_t peripheral;
    uint32_t clock;
    Gpio* scl;
    Gpio* sda;
};

#endif /* I2C_H_ */
