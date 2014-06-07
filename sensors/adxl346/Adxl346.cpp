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

#define ADXL346_ADDRESS             ( 0x53 )

#define ADXL346_DEVID_ADDR          ( 0x00 )
#define ADXL346_DEVID_VALUE         ( 0xE6 )

#define ADXL346_THRES_TAP_ADDR      ( 0x1D )
#define ADXL346_OFSX_ADDR           ( 0x1E )
#define ADXL346_OFSY_ADDR           ( 0x1F )
#define ADXL346_OFSZ_ADDR           ( 0x20 )
#define ADXL346_DUR_ADDR            ( 0x21 )
#define ADXL346_LATENT_ADDR         ( 0x22 )
#define ADXL346_WINDOW_ADDR         ( 0x23 )
#define ADXL346_THRESH_ACT_ADDR     ( 0x24 )
#define ADXL346_THRESH_INACT_ADDR   ( 0x25 )
#define ADXL346_TIME_INACT_ADDR     ( 0x26 )
#define ADXL346_ACT_INACT_CTL_ADDR  ( 0x27 )
#define ADXL346_THRESH_FF_ADDR      ( 0x28 )
#define ADXL346_TIME_FF_ADDR        ( 0x29 )
#define ADXL346_TAP_AXES_ADDR       ( 0x2A )
#define ADXL346_ACT_TAP_STATUS_ADDR ( 0x2B )
#define ADXL346_BW_RATE_ADDR        ( 0x2C )
#define ADXL346_POWER_CTL_ADDR      ( 0x2D )
#define ADXL346_INT_ENABLE_ADDR     ( 0x2E )
#define ADXL346_INT_ADDR_ADDR       ( 0x2F )
#define ADXL346_INT_SOURCE_ADDR     ( 0x30 )
#define ADXL346_DATA_FORMAT_ADDR    ( 0x31 )
#define ADXL346_DATAX0_ADDR         ( 0x32 )
#define ADXL346_DATAX1_ADDR         ( 0x33 )
#define ADXL346_DATAY0_ADDR         ( 0x34 )
#define ADXL346_DATAY1_ADDR         ( 0x35 )
#define ADXL346_DATAZ0_ADDR         ( 0x36 )
#define ADXL346_DATAZ1_ADDR         ( 0x37 )
#define ADXL346_FIFO_CTL_ADDR       ( 0x38 )
#define ADXL346_FIFO_STATUS_ADDR    ( 0x39 )
#define ADXL346_TAP_SIGN_ADDR       ( 0x3A )
#define ADXL346_ORIENT_CONF_ADDR    ( 0x3B )
#define ADXL346_ORIENT_ADDR         ( 0x3C )



Adxl346::Adxl346(I2c* i2c_):
    i2c(i2c_)
{
}

void Adxl346::enable(void)
{
}

void Adxl346::reset(void)
{
}

bool Adxl346::isPresent(void)
{
    uint8_t status;
    status = i2c->readByte(ADXL346_ADDRESS, ADXL346_DEVID_ADDR);
    return (status == ADXL346_DEVID_VALUE);
}

void Adxl346::readAcceleration(void)
{
    uint8_t status;
    uint8_t acceleration[6];
    status = i2c->readByte(ADXL346_ADDRESS, ADXL346_DATAX0_ADDR, acceleration, sizeof(acceleration));
    x = (acceleration[0] << 8) | acceleration[1];
    y = (acceleration[2] << 8) | acceleration[3];
    z = (acceleration[4] << 8) | acceleration[5];
}

uint16_t Adxl346::getX(void)
{
    return x;
}

uint16_t Adxl346::getY(void)
{
    return y;
}

uint16_t Adxl346::getZ(void)
{
    return z;
}
