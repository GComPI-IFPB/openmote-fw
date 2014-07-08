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
#include "GpioIn.h"

#define MAX44009_ADDRESS                    ( 0x4A )
#define MAX44009_NOT_FOUND                  ( 0x00 )

#define MAX44009_INT_STATUS_ADDR            ( 0x00 )    // R
#define MAX44009_IN_ENABLE_ADDR             ( 0x01 )    // R/W
#define MAX44009_CONFIG_ADDR                ( 0x02 )    // R/W
#define MAX44009_LUX_HIGH_ADDR              ( 0x03 )    // R
#define MAX44009_LUX_LOW_ADDR               ( 0x04 )    // R
#define MAX44009_THR_HIGH_ADDR              ( 0x05 )    // R/W
#define MAX44009_THR_LOW_ADDR               ( 0x06 )    // R/W
#define MAX44009_THR_TIMER_ADDR             ( 0x07 )    // R/W

#define MAX44009_INT_STATUS_OFF             ( 0x00 )
#define MAX44009_INT_STATUS_ON              ( 0x01 )
#define MAX44009_INT_DISABLED               ( 0x00 )
#define MAX44009_INT_ENABLED                ( 0x01 )

#define MAX44009_CONFIG_DEFAULT             ( 0 << 7 )
#define MAX44009_CONFIG_CONTINUOUS          ( 1 << 7 )
#define MAX44009_CONFIG_AUTO                ( 0 << 6 )
#define MAX44009_CONFIG_MANUAL              ( 1 << 6 )
#define MAX44009_CONFIG_CDR_NORMAL          ( 0 << 5 )
#define MAX44009_CONFIG_CDR_DIVIDED         ( 1 << 5 )
#define MAX44009_CONFIG_INTEGRATION_800ms   ( 0 << 0 )
#define MAX44009_CONFIG_INTEGRATION_400ms   ( 1 << 0 )
#define MAX44009_CONFIG_INTEGRATION_200ms   ( 2 << 0 )
#define MAX44009_CONFIG_INTEGRATION_100ms   ( 3 << 0 )
#define MAX44009_CONFIG_INTEGRATION_50ms    ( 4 << 0 )
#define MAX44009_CONFIG_INTEGRATION_25ms    ( 5 << 0 )
#define MAX44009_CONFIG_INTEGRATION_12ms    ( 6 << 0 )
#define MAX44009_CONFIG_INTEGRATION_6ms     ( 7 << 0 )

#define MAX44009_DEFAULT_CONFIGURATION      ( MAX44009_CONFIG_DEFAULT | \
                                              MAX44009_CONFIG_AUTO | \
                                              MAX44009_CONFIG_CDR_NORMAL | \
                                              MAX44009_CONFIG_INTEGRATION_100ms )

Max44009::Max44009(I2c* i2c_, GpioIn* gpio_):
    i2c(i2c_), gpio(gpio_)
{
}

void Max44009::enable(void)
{
}

void Max44009::reset(void)
{
}

void Max44009::setCallback(callback_t callback)
{
    gpio->setCallback(callback);
    gpio->enableInterrupt();
}

void Max44009::clearCallback(void)
{
    gpio->clearCallback();
    gpio->disableInterrupt();
}

bool Max44009::isPresent(void)
{
    uint8_t status;
    
    i2c->lock();
    status = i2c->readByte(MAX44009_ADDRESS, MAX44009_CONFIG_ADDR);
    i2c->unlock();
        
    return (status != MAX44009_NOT_FOUND);
}

void Max44009::readLux(void)
{
    uint8_t max44009_data[2];

    i2c->lock();
    i2c->readByte(MAX44009_ADDRESS, MAX44009_LUX_HIGH_ADDR, max44009_data, sizeof(max44009_data));
    i2c->unlock();
    
    exponent = (( max44009_data[0] >> 4 )  & 0x0E);
    mantissa = (( max44009_data[0] & 0x0F) << 4) | (max44009_data[1] & 0x0F);
}

float Max44009::getLux(void)
{
    float lux = 0.045;
    lux *= 2^exponent * mantissa;
    return lux;
}

uint16_t Max44009::getLuxRaw(void)
{
    uint16_t lux;
    lux = ( (uint16_t)exponent << 8 ) | ( (uint16_t) mantissa );
    return lux;
}
