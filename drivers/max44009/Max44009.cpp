/*
 * Copyright 2013 OpenMote Technologies, S.L.
 */

/**
 *
 * @file       Max44009.cpp
 * @author     Pere Tuset-Peiro (peretuset@openmote.com)
 * @version    v0.1
 * @date       May, 2014
 * @brief
 * @ingroup
 *
 */

/*================================ include ==================================*/

#include "Max44009.h"

#include "I2cDriver.h"
#include "GpioIn.h"

/*================================ define ===================================*/

/* ADDRESS AND NOT_FOUND VALUE */
#define MAX44009_ADDRESS                    ( 0x4A )
#define MAX44009_NOT_FOUND                  ( 0x00 )

/* REGISTER ADDRESSES */
#define MAX44009_INT_STATUS_ADDR            ( 0x00 )    // R
#define MAX44009_INT_ENABLE_ADDR            ( 0x01 )    // R/W
#define MAX44009_CONFIG_ADDR                ( 0x02 )    // R/W
#define MAX44009_LUX_HIGH_ADDR              ( 0x03 )    // R
#define MAX44009_LUX_LOW_ADDR               ( 0x04 )    // R
#define MAX44009_THR_HIGH_ADDR              ( 0x05 )    // R/W
#define MAX44009_THR_LOW_ADDR               ( 0x06 )    // R/W
#define MAX44009_THR_TIMER_ADDR             ( 0x07 )    // R/W

/* INTERRUPT VALUES */
#define MAX44009_INT_STATUS_OFF             ( 0x00 )
#define MAX44009_INT_STATUS_ON              ( 0x01 )
#define MAX44009_INT_DISABLED               ( 0x00 )
#define MAX44009_INT_ENABLED                ( 0x01 )

/* CONFIGURATION VALUES */
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

/* DEFAULT CONFIGURATION */
#define MAX44009_DEFAULT_CONFIGURATION      ( MAX44009_CONFIG_DEFAULT | \
                                              MAX44009_CONFIG_AUTO | \
                                              MAX44009_CONFIG_CDR_NORMAL | \
                                              MAX44009_CONFIG_INTEGRATION_100ms )

/*================================ typedef ==================================*/

/*=============================== variables =================================*/

/*=============================== prototypes ================================*/

/*================================= public ==================================*/

Max44009::Max44009(I2cDriver& i2c_, GpioIn& gpio_):
    i2c(i2c_), gpio(gpio_)
{
}

bool Max44009::enable(void)
{
    uint8_t max44009_address[5] = {MAX44009_INT_ENABLE_ADDR, MAX44009_CONFIG_ADDR, \
                                   MAX44009_THR_HIGH_ADDR, MAX44009_THR_LOW_ADDR, \
                                   MAX44009_THR_TIMER_ADDR};
    uint8_t max44009_value[5];
    uint8_t max44009_data[2];
    bool status;

    max44009_value[0] = (MAX44009_INT_STATUS_ON);
    max44009_value[1] = (MAX44009_DEFAULT_CONFIGURATION);
    max44009_value[2] = (0xFF);
    max44009_value[3] = (0x00);
    max44009_value[4] = (0xFF);

    i2c.lock();

    for(uint8_t i = 0; i < sizeof(max44009_address); i++)
    {
        max44009_data[0] = max44009_value[i];
        max44009_data[1] = max44009_data[i];
        status = i2c.writeByte(MAX44009_ADDRESS, max44009_data, 2);
        if (status == false)
        {
            i2c.unlock();
            return status;
        }
    }

    i2c.unlock();

    return true;
}

bool Max44009::reset(void)
{
    uint8_t max44009_address[5] = {MAX44009_INT_ENABLE_ADDR, MAX44009_CONFIG_ADDR, \
                                   MAX44009_THR_HIGH_ADDR, MAX44009_THR_LOW_ADDR, \
                                   MAX44009_THR_TIMER_ADDR};
    uint8_t max44009_value[5] = {0x00, 0x03, 0xFF, 0x00, 0xFF};
    uint8_t max44009_data[2];
    bool status;

    i2c.lock();

    for(uint8_t i = 0; i < sizeof(max44009_address); i++)
    {
        max44009_data[0] = max44009_value[i];
        max44009_data[1] = max44009_data[i];
        status = i2c.writeByte(MAX44009_ADDRESS, max44009_data, 2);
        if (status == false)
        {
            i2c.unlock();
            return status;
        }
    }

    i2c.unlock();
    return true;
}

void Max44009::setCallback(Callback* callback_)
{
    gpio.setCallback(callback_);
    gpio.enableInterrupt();
}

void Max44009::clearCallback(void)
{
    gpio.clearCallback();
    gpio.disableInterrupt();
}

bool Max44009::isPresent(void)
{
    bool status;
    uint8_t isPresent;

    i2c.lock();
    status = i2c.writeByte(MAX44009_ADDRESS, MAX44009_CONFIG_ADDR);
    status = i2c.readByte(MAX44009_ADDRESS, &isPresent);
    i2c.unlock();

    return (status && isPresent != MAX44009_NOT_FOUND);
}

bool Max44009::readLux(void)
{
    bool status;
    uint8_t max44009_data[2];

    i2c.lock();
    status = i2c.writeByte(MAX44009_ADDRESS, MAX44009_LUX_HIGH_ADDR);
    status = i2c.readByte(MAX44009_ADDRESS, &max44009_data[0]);
    status = i2c.writeByte(MAX44009_ADDRESS, MAX44009_LUX_LOW_ADDR);
    status = i2c.readByte(MAX44009_ADDRESS, &max44009_data[1]);
    i2c.unlock();

    if (status)
    {
        exponent = (( max44009_data[0] >> 4 )  & 0x0E);
        mantissa = (( max44009_data[0] & 0x0F) << 4) | (max44009_data[1] & 0x0F);
    }

    return status;
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

/*=============================== protected =================================*/

/*================================ private ==================================*/
