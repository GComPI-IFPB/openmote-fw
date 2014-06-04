/*
 * Copyright 2013 OpenMote Technologies, S.L.
 */

/**
 *
 * @file       max44009.cpp
 * @author     Pere Tuset-Peiro (peretuset@openmote.com)
 * @version    v0.1
 * @date       May, 2014
 * @brief
 * @ingroup
 *
 */

#include "Max44009.h"
#include "I2c.h"

#define MAX44009_ADDRESS            ( 0x4A )

Max44009::Max44009(I2c* i2c_):
    i2c(i2c_)
{
}

bool Max44009::isPresent(void)
{
    uint8_t status;
    status = i2c->readByte(MAX44009_ADDRESS, MAX44009_REGISTER);
    return (status == MAX44009_VALUE);
}
