/*
 * Copyright 2013 OpenMote Technologies, S.L.
 */

/**
 *
 * @file       adxl346.h
 * @author     Pere Tuset-Peiro (peretuset@openmote.com)
 * @version    v0.1
 * @date       May, 2014
 * @brief
 * @ingroup
 *
 */

#ifndef ADXL346_H_
#define ADXL346_H_

#include <stdint.h>

#include "Sensor.h"

typedef void (*callback_t)(void);

class I2cDriver;
class GpioIn;

class Adxl346: public Sensor
{

public:
    Adxl346(I2cDriver* i2c_, GpioIn* gpio_);
    void enable(void);
    void reset(void);
    bool isPresent(void);
    void setCallback(callback_t callback);
    void clearCallback(void);
    void readAcceleration(void);
    uint16_t getX(void);
    uint16_t getY(void);
    uint16_t getZ(void);
private:
    I2cDriver * i2c;
    GpioIn * gpio;
    uint16_t x;
    uint16_t y;
    uint16_t z;
};

#endif /* ADXL346_H_ */
