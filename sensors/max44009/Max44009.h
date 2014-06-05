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

#include "Sensor.h"

class I2c;

class Max44009: public Sensor
{

public:
    Max44009(I2c* i2c_);
    bool isPresent(void);
private:
    I2c * i2c;
};

#endif /* MAX44009_H_ */
