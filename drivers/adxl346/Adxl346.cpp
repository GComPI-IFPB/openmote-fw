/*
 * Copyright 2013 OpenMote Technologies, S.L.
 */

/**
 *
 * @file       Adxl346.cpp
 * @author     Pere Tuset-Peiro (peretuset@openmote.com)
 * @version    v0.1
 * @date       May, 2014
 * @brief
 * @ingroup
 *
 */

/*================================ include ==================================*/

#include "Adxl346.h"

#include "I2cDriver.h"
#include "GpioIn.h"

/*================================ define ===================================*/

/* ADDRESS AND IDENTIFIER */
#define ADXL346_ADDRESS                     ( 0x53 )
#define ADXL346_DEVID_VALUE                 ( 0xE6 )

/* REGISTER ADDRESSES */
#define ADXL346_DEVID_ADDR                  ( 0x00 )
#define ADXL346_THRES_TAP_ADDR              ( 0x1D )
#define ADXL346_OFSX_ADDR                   ( 0x1E )
#define ADXL346_OFSY_ADDR                   ( 0x1F )
#define ADXL346_OFSZ_ADDR                   ( 0x20 )
#define ADXL346_DUR_ADDR                    ( 0x21 )
#define ADXL346_LATENT_ADDR                 ( 0x22 )
#define ADXL346_WINDOW_ADDR                 ( 0x23 )
#define ADXL346_THRESH_ACT_ADDR             ( 0x24 )
#define ADXL346_THRESH_INACT_ADDR           ( 0x25 )
#define ADXL346_TIME_INACT_ADDR             ( 0x26 )
#define ADXL346_ACT_INACT_CTL_ADDR          ( 0x27 )
#define ADXL346_THRESH_FF_ADDR              ( 0x28 )
#define ADXL346_TIME_FF_ADDR                ( 0x29 )
#define ADXL346_TAP_AXES_ADDR               ( 0x2A )
#define ADXL346_ACT_TAP_STATUS_ADDR         ( 0x2B )
#define ADXL346_BW_RATE_ADDR                ( 0x2C )
#define ADXL346_POWER_CTL_ADDR              ( 0x2D )
#define ADXL346_INT_ENABLE_ADDR             ( 0x2E )
#define ADXL346_INT_MAP_ADDR                ( 0x2F )
#define ADXL346_INT_SOURCE_ADDR             ( 0x30 )
#define ADXL346_DATA_FORMAT_ADDR            ( 0x31 )
#define ADXL346_DATAX0_ADDR                 ( 0x32 )
#define ADXL346_DATAX1_ADDR                 ( 0x33 )
#define ADXL346_DATAY0_ADDR                 ( 0x34 )
#define ADXL346_DATAY1_ADDR                 ( 0x35 )
#define ADXL346_DATAZ0_ADDR                 ( 0x36 )
#define ADXL346_DATAZ1_ADDR                 ( 0x37 )
#define ADXL346_FIFO_CTL_ADDR               ( 0x38 )
#define ADXL346_FIFO_STATUS_ADDR            ( 0x39 )
#define ADXL346_TAP_SIGN_ADDR               ( 0x3A )
#define ADXL346_ORIENT_CONF_ADDR            ( 0x3B )
#define ADXL346_ORIENT_ADDR                 ( 0x3C )

/* INT_ENABLE/INT_MAP/INT_SOURCE */
#define ADXL346_INT_ENABLE_DATA_READY      ( 1 << 7 )
#define ADXL346_INT_ENABLE_SINGLE_TAP      ( 1 << 6 )
#define ADXL346_INT_ENABLE_DOUBLE_TAP      ( 1 << 5 )
#define ADXL346_INT_ENABLE_ACTIVITY        ( 1 << 4 )
#define ADXL346_INT_ENABLE_INACTIVITY      ( 1 << 3 )
#define ADXL346_INT_ENABLE_FREE_FALL       ( 1 << 2 )
#define ADXL346_INT_ENABLE_WATERMARK       ( 1 << 1 )
#define ADXL346_INT_ENABLE_OVERRUN         ( 1 << 0 )

/* ACT_INACT_CONTROL */
#define ADXL346_ACT_INACT_CTL_ACT_ACDC     ( 1 << 7 )
#define ADXL346_ACT_INACT_CTL_ACT_X_EN     ( 1 << 6 )
#define ADXL346_ACT_INACT_CTL_ACT_Y_EN     ( 1 << 5 )
#define ADXL346_ACT_INACT_CTL_ACT_Z_EN     ( 1 << 4 )
#define ADXL346_ACT_INACT_CTL_INACT_ACDC   ( 1 << 3 )
#define ADXL346_ACT_INACT_CTL_INACT_X_EN   ( 1 << 2 )
#define ADXL346_ACT_INACT_CTL_INACT_Y_EN   ( 1 << 1 )
#define ADXL346_ACT_INACT_CTL_INACT_Z_EN   ( 1 << 0 )

/* TAP_AXES */
#define ADXL346_TAP_AXES_SUPPRESS           ( 1 << 3 )
#define ADXL346_TAP_AXES_TAP_X_EN           ( 1 << 2 )
#define ADXL346_TAP_AXES_TAP_Y_EN           ( 1 << 1 )
#define ADXL346_TAP_AXES_TAP_Z_EN           ( 1 << 0 )

/* ACT_TAP_STATUS */
#define ADXL346_ACT_TAP_STATUS_ACT_X_SRC    ( 1 << 6 )
#define ADXL346_ACT_TAP_STATUS_ACT_Y_SRC    ( 1 << 5 )
#define ADXL346_ACT_TAP_STATUS_ACT_Z_SRC    ( 1 << 4 )
#define ADXL346_ACT_TAP_STATUS_ASLEEP       ( 1 << 3 )
#define ADXL346_ACT_TAP_STATUS_TAP_X_SRC    ( 1 << 2 )
#define ADXL346_ACT_TAP_STATUS_TAP_Y_SRC    ( 1 << 1 )
#define ADXL346_ACT_TAP_STATUS_TAP_Z_SRC    ( 1 << 0 )

/* BW_RATE */
#define ADXL346_BW_RATE_POWER               ( 1 << 4 )
#define ADXL346_BW_RATE_RATE(x)             ( (x) & 0x0F)

/* POWER CONTROL */
#define ADXL346_POWER_CTL_LINK              ( 1 << 5 )
#define ADXL346_POWER_CTL_AUTO_SLEEP        ( 1 << 4 )
#define ADXL346_POWER_CTL_MEASURE           ( 1 << 3 )
#define ADXL346_POWER_CTL_SLEEP             ( 1 << 2 )
#define ADXL346_POWER_CTL_WAKEUP(x)         ( (x) & 0x03 )

/* DATA_FORMAT */
#define ADXL346_DATA_FORMAT_SELF_TEST       ( 1 << 7 )
#define ADXL346_DATA_FORMAT_SPI             ( 1 << 6 )
#define ADXL346_DATA_FORMAT_INT_INVERT      ( 1 << 5 )
#define ADXL346_DATA_FORMAT_FULL_RES        ( 1 << 3 )
#define ADXL346_DATA_FORMAT_JUSTIFY         ( 1 << 2 )
#define ADXL346_DATA_FORMAT_RANGE(x)        ( (x) & 0x03 )
#define ADXL346_DATA_FORMAT_RANGE_PM_2g     ( 0 )
#define ADXL346_DATA_FORMAT_RANGE_PM_4g     ( 1 )
#define ADXL346_DATA_FORMAT_RANGE_PM_8g     ( 2 )
#define ADXL346_DATA_FORMAT_RANGE_PM_16g    ( 3 )

/*================================ typedef ==================================*/

/*=============================== variables =================================*/

/*=============================== prototypes ================================*/

/*================================= public ==================================*/

Adxl346::Adxl346(I2cDriver& i2c_, GpioIn& gpio_):
    i2c(i2c_), gpio(gpio_)
{
}

bool Adxl346::enable(void)
{
    bool status;
    uint8_t config[2];

    i2c.lock();

    config[0] = ADXL346_BW_RATE_ADDR;
    config[1] = (ADXL346_BW_RATE_RATE(11));
    status = i2c.writeByte(ADXL346_ADDRESS, config, sizeof(config));

    if (!status) goto error;

    config[0] = ADXL346_DATA_FORMAT_ADDR;
    config[1] = (ADXL346_DATA_FORMAT_SELF_TEST |
                 ADXL346_DATA_FORMAT_FULL_RES  |
                 ADXL346_DATA_FORMAT_RANGE_PM_16g);
    status = i2c.writeByte(ADXL346_ADDRESS, config, sizeof(config));

    if (!status) goto error;

    config[0] = ADXL346_POWER_CTL_ADDR;
    config[1] = ADXL346_POWER_CTL_MEASURE;
    status = i2c.writeByte(ADXL346_ADDRESS, config, sizeof(config));

    if (!status) goto error;

    i2c.unlock();

    return status;

error:
    i2c.unlock();
    return status;
}

bool Adxl346::reset(void)
{
    return false;
}

bool Adxl346::isPresent(void)
{
    bool status;
    uint8_t isPresent;

    i2c.lock();
    status = i2c.writeByte(ADXL346_ADDRESS, ADXL346_DEVID_ADDR);
    status = i2c.readByte(ADXL346_ADDRESS, &isPresent);
    i2c.unlock();

    return (status && isPresent == ADXL346_DEVID_VALUE);
}

void Adxl346::setCallback(Callback* callback_)
{
    gpio.setCallback(callback_);
    gpio.enableInterrupts();
}

void Adxl346::clearCallback(void)
{
    gpio.clearCallback();
    gpio.disableInterrupts();
}

bool Adxl346::readAcceleration(void)
{
    bool status;
    uint8_t acceleration[2];

    i2c.lock();

    status = i2c.writeByte(ADXL346_ADDRESS, ADXL346_DATAX0_ADDR);
    status = i2c.readByte(ADXL346_ADDRESS, &acceleration[0]);
    status = i2c.writeByte(ADXL346_ADDRESS, ADXL346_DATAX1_ADDR);
    status = i2c.readByte(ADXL346_ADDRESS, &acceleration[1]);

    if (status) {
        x = (acceleration[0] << 8) | acceleration[1];
    } else {
        goto error;
    }

    status = i2c.writeByte(ADXL346_ADDRESS, ADXL346_DATAY0_ADDR);
    status = i2c.readByte(ADXL346_ADDRESS, &acceleration[0]);
    status = i2c.writeByte(ADXL346_ADDRESS, ADXL346_DATAY1_ADDR);
    status = i2c.readByte(ADXL346_ADDRESS, &acceleration[1]);

    if (status) {
        y = (acceleration[0] << 8) | acceleration[1];
    } else {
        goto error;
    }

    status = i2c.writeByte(ADXL346_ADDRESS, ADXL346_DATAZ0_ADDR);
    status = i2c.readByte(ADXL346_ADDRESS, &acceleration[0]);
    status = i2c.writeByte(ADXL346_ADDRESS, ADXL346_DATAZ1_ADDR);
    status = i2c.readByte(ADXL346_ADDRESS, &acceleration[1]);

    if (status) {
        z = (acceleration[0] << 8) | acceleration[1];
    } else {
        goto error;
    }

    i2c.unlock();

    return status;

error:
    x = 0; y = 0; z = 0;
    i2c.unlock();
    return status;
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

/*=============================== protected =================================*/

/*================================ private ==================================*/
