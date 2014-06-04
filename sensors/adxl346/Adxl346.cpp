/*
 * Copyright 2013 OpenMote Technologies, S.L.
 */

/**
 *
 * @file       adxl346.cpp
 * @author     Pere Tuset-Peiro (peretuset@openmote.com)
 * @version    v0.1
 * @date       May, 2014
 * @brief
 * @ingroup
 *
 */

#include "Adxl346.h"
#include "I2c.h"

#define ADLX346_ADDRESS             ( 0x53 )
#define ADXL346_REGISTER            ( 0x00 )
#define ADXL346_VALUE               ( 0xE6 )

Adxl346::Adxl346(I2c* i2c_):
    i2c(i2c_)
{
}

bool Adxl346::isPresent(void)
{
    uint8_t status;
    status = i2c->readByte(ADLX346_ADDRESS, ADXL346_REGISTER);
    return (status == ADXL346_VALUE);
}
