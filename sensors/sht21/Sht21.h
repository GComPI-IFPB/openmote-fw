/*
 * Copyright 2013 OpenMote Technologies, S.L.
 */

/**
 *
 * @file       sht21.h
 * @author     Pere Tuset-Peiro (peretuset@openmote.com)
 * @version    v0.1
 * @date       May, 2014
 * @brief
 * @ingroup
 *
 */

#ifndef SHT21_H_
#define SHT21_H_

#include "hw_types.h"

#include "Sensor.h"

class I2c;

class Sht21: public Sensor
{
public:
    Sht21(I2c* i2c_);
    bool isPresent(void);
    void reset(void);
    void readTemperature(void);
    void readHumidity(void);
    float getTemperature(void);
    float getHumidity(void);
private:
    I2c* i2c;
    uint16_t temperature;
    uint16_t humidity;
};

#endif /* SHT21_H_ */
