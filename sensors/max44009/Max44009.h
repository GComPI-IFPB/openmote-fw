/*
 * Copyright 2013 OpenMote Technologies, S.L.
 */

/**
 *
 * @file       max44009.h
 * @author     Pere Tuset-Peiro (peretuset@openmote.com)
 * @version    v0.1
 * @date       May, 2014
 * @brief
 * @ingroup
 *
 */

#ifndef MAX44009_H_
#define MAX44009_H_

#include <stdint.h>

#include "Sensor.h"

typedef void (*callback_t)(void);

class I2c;
class GpioIn;

class Max44009: public Sensor
{

public:
    Max44009(I2c* i2c_, GpioIn* gpio_);
    void enable(void);
    void reset(void);
    void setCallback(callback_t callback);
    void clearCallback(void);
    bool isPresent(void);
    void readLux(void);
    float getLux(void);
    uint16_t getLuxRaw(void);
private:
    I2c * i2c;
    GpioIn * gpio;
    uint8_t exponent;
    uint8_t mantissa;
};

#endif /* MAX44009_H_ */
