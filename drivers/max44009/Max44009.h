/*
 * Copyright 2013 OpenMote Technologies, S.L.
 */

/**
 *
 * @file       Max44009.h
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
#include "Callback.h"

class I2cDriver;
class GpioIn;

class Max44009: public Sensor
{
public:
    Max44009(I2cDriver& i2c, GpioIn& gpio);
    bool enable(void);
    bool reset(void);
    void setCallback(Callback* callback);
    void clearCallback(void);
    bool isPresent(void);
    bool readLux(void);
    float getLux(void);
    uint16_t getLuxRaw(void);
private:
    I2cDriver& i2c_;
    GpioIn& gpio_;

    uint8_t exponent;
    uint8_t mantissa;
};

#endif /* MAX44009_H_ */
