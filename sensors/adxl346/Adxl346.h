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

class I2c;

class Adxl346
{

public:
    Adxl346(I2c* i2c_);
    bool isPresent(void);
private:
    I2c * i2c;
};

#endif /* ADXL346_H_ */
