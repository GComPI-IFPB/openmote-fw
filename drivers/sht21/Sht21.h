/*
 * Copyright 2013 OpenMote Technologies, S.L.
 */

/**
 *
 * @file       Sht21.h
 * @author     Pere Tuset-Peiro (peretuset@openmote.com)
 * @version    v0.1
 * @date       May, 2014
 * @brief
 * @ingroup
 *
 */

#ifndef SHT21_H_
#define SHT21_H_

#include <stdint.h>

#include "Sensor.h"

class I2cDriver;

class Sht21: public Sensor
{
public:
    Sht21(I2cDriver* i2c_);
    bool enable(void);
    bool reset(void);
    bool isPresent(void);
    bool readTemperature(void);
    bool readHumidity(void);
    float getTemperature(void);
    uint16_t getTemperatureRaw(void);
    float getHumidity(void);
    uint16_t getHumidityRaw(void);
private:
    I2cDriver* i2c;
    uint16_t temperature;
    uint16_t humidity;
};

#endif /* SHT21_H_ */
