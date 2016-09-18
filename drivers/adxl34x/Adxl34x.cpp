/**
 * @file       Adxl34x.cpp
 * @author     Pere Tuset-Peiro (peretuset@openmote.com)
 * @version    v0.1
 * @date       May, 2015
 * @brief
 *
 * @copyright  Copyright 2015, OpenMote Technologies, S.L.
 *             This file is licensed under the GNU General Public License v2.
 */

/*================================ include ==================================*/

#include "Adxl34x.h"
 
#include "platform_types.h"

/*================================ define ===================================*/

/* ADDRESS AND IDENTIFIER */
#define ADXL34X_ADDRESS                     ( 0x53 )
#define ADXL34X_DEVID_VALUE                 ( 0xE6 )

/* REGISTER ADDRESSES */
#define ADXL34X_DEVID_ADDR                  ( 0x00 )
#define ADXL34X_THRES_TAP_ADDR              ( 0x1D )
#define ADXL34X_OFSX_ADDR                   ( 0x1E )
#define ADXL34X_OFSY_ADDR                   ( 0x1F )
#define ADXL34X_OFSZ_ADDR                   ( 0x20 )
#define ADXL34X_DUR_ADDR                    ( 0x21 )
#define ADXL34X_LATENT_ADDR                 ( 0x22 )
#define ADXL34X_WINDOW_ADDR                 ( 0x23 )
#define ADXL34X_THRESH_ACT_ADDR             ( 0x24 )
#define ADXL34X_THRESH_INACT_ADDR           ( 0x25 )
#define ADXL34X_TIME_INACT_ADDR             ( 0x26 )
#define ADXL34X_ACT_INACT_CTL_ADDR          ( 0x27 )
#define ADXL34X_THRESH_FF_ADDR              ( 0x28 )
#define ADXL34X_TIME_FF_ADDR                ( 0x29 )
#define ADXL34X_TAP_AXES_ADDR               ( 0x2A )
#define ADXL34X_ACT_TAP_STATUS_ADDR         ( 0x2B )
#define ADXL34X_BW_RATE_ADDR                ( 0x2C )
#define ADXL34X_POWER_CTL_ADDR              ( 0x2D )
#define ADXL34X_INT_ENABLE_ADDR             ( 0x2E )
#define ADXL34X_INT_MAP_ADDR                ( 0x2F )
#define ADXL34X_INT_SOURCE_ADDR             ( 0x30 )
#define ADXL34X_DATA_FORMAT_ADDR            ( 0x31 )
#define ADXL34X_DATAX0_ADDR                 ( 0x32 )
#define ADXL34X_DATAX1_ADDR                 ( 0x33 )
#define ADXL34X_DATAY0_ADDR                 ( 0x34 )
#define ADXL34X_DATAY1_ADDR                 ( 0x35 )
#define ADXL34X_DATAZ0_ADDR                 ( 0x36 )
#define ADXL34X_DATAZ1_ADDR                 ( 0x37 )
#define ADXL34X_FIFO_CTL_ADDR               ( 0x38 )
#define ADXL34X_FIFO_STATUS_ADDR            ( 0x39 )
#define ADXL34X_TAP_SIGN_ADDR               ( 0x3A )
#define ADXL34X_ORIENT_CONF_ADDR            ( 0x3B )
#define ADXL34X_ORIENT_ADDR                 ( 0x3C )

/* INT_ENABLE/INT_MAP/INT_SOURCE */
#define ADXL34X_INT_ENABLE_DATA_READY      ( 1 << 7 )
#define ADXL34X_INT_ENABLE_SINGLE_TAP      ( 1 << 6 )
#define ADXL34X_INT_ENABLE_DOUBLE_TAP      ( 1 << 5 )
#define ADXL34X_INT_ENABLE_ACTIVITY        ( 1 << 4 )
#define ADXL34X_INT_ENABLE_INACTIVITY      ( 1 << 3 )
#define ADXL34X_INT_ENABLE_FREE_FALL       ( 1 << 2 )
#define ADXL34X_INT_ENABLE_WATERMARK       ( 1 << 1 )
#define ADXL34X_INT_ENABLE_OVERRUN         ( 1 << 0 )

/* ACT_INACT_CONTROL */
#define ADXL34X_ACT_INACT_CTL_ACT_ACDC     ( 1 << 7 )
#define ADXL34X_ACT_INACT_CTL_ACT_X_EN     ( 1 << 6 )
#define ADXL34X_ACT_INACT_CTL_ACT_Y_EN     ( 1 << 5 )
#define ADXL34X_ACT_INACT_CTL_ACT_Z_EN     ( 1 << 4 )
#define ADXL34X_ACT_INACT_CTL_INACT_ACDC   ( 1 << 3 )
#define ADXL34X_ACT_INACT_CTL_INACT_X_EN   ( 1 << 2 )
#define ADXL34X_ACT_INACT_CTL_INACT_Y_EN   ( 1 << 1 )
#define ADXL34X_ACT_INACT_CTL_INACT_Z_EN   ( 1 << 0 )

/* TAP_AXES */
#define ADXL34X_TAP_AXES_SUPPRESS           ( 1 << 3 )
#define ADXL34X_TAP_AXES_TAP_X_EN           ( 1 << 2 )
#define ADXL34X_TAP_AXES_TAP_Y_EN           ( 1 << 1 )
#define ADXL34X_TAP_AXES_TAP_Z_EN           ( 1 << 0 )

/* ACT_TAP_STATUS */
#define ADXL34X_ACT_TAP_STATUS_ACT_X_SRC    ( 1 << 6 )
#define ADXL34X_ACT_TAP_STATUS_ACT_Y_SRC    ( 1 << 5 )
#define ADXL34X_ACT_TAP_STATUS_ACT_Z_SRC    ( 1 << 4 )
#define ADXL34X_ACT_TAP_STATUS_ASLEEP       ( 1 << 3 )
#define ADXL34X_ACT_TAP_STATUS_TAP_X_SRC    ( 1 << 2 )
#define ADXL34X_ACT_TAP_STATUS_TAP_Y_SRC    ( 1 << 1 )
#define ADXL34X_ACT_TAP_STATUS_TAP_Z_SRC    ( 1 << 0 )

/* BW_RATE */
#define ADXL34X_BW_RATE_LOW_POWER           ( 1 << 4 )
#define ADXL34X_BW_RATE_RATE(x)             ( (x) & 0x0F)

/* POWER CONTROL */
#define ADXL34X_POWER_CTL_LINK              ( 1 << 5 )
#define ADXL34X_POWER_CTL_AUTO_SLEEP        ( 1 << 4 )
#define ADXL34X_POWER_CTL_MEASURE           ( 1 << 3 )
#define ADXL34X_POWER_CTL_SLEEP             ( 1 << 2 )
#define ADXL34X_POWER_CTL_WAKEUP(x)         ( (x) & 0x03 )

/* DATA_FORMAT */
#define ADXL34X_DATA_FORMAT_SELF_TEST       ( 1 << 7 )
#define ADXL34X_DATA_FORMAT_SPI             ( 1 << 6 )
#define ADXL34X_DATA_FORMAT_INT_INVERT      ( 1 << 5 )
#define ADXL34X_DATA_FORMAT_FULL_RES        ( 1 << 3 )
#define ADXL34X_DATA_FORMAT_JUSTIFY         ( 1 << 2 )
#define ADXL34X_DATA_FORMAT_RANGE_PM_2g     ( 0 )
#define ADXL34X_DATA_FORMAT_RANGE_PM_4g     ( 1 )
#define ADXL34X_DATA_FORMAT_RANGE_PM_8g     ( 2 )
#define ADXL34X_DATA_FORMAT_RANGE_PM_16g    ( 3 )

/*================================ typedef ==================================*/

/*=============================== variables =================================*/

/*=============================== prototypes ================================*/

/*================================= public ==================================*/

Adxl34x::Adxl34x(I2c& i2c, GpioIn& gpio):
    i2c_(i2c), gpio_(gpio)
{
}

bool Adxl34x::enable(void)
{
    uint8_t config[2];
    uint8_t buffer[32];
    uint8_t samples;
    bool status;

    // Lock access to I2C
    i2c_.lock();

    // Clear the power register
    config[0] = ADXL34X_POWER_CTL_ADDR;
    config[1] = 0x00;
    status = i2c_.writeByte(ADXL34X_ADDRESS, config, sizeof(config));
    if (status == false) goto error;

    // Read the FIFO status register
    status = i2c_.writeByte(ADXL34X_ADDRESS, ADXL34X_FIFO_STATUS_ADDR);
    if (status == false) goto error;
    status = i2c_.readByte(ADXL34X_ADDRESS, &samples);
    if (status == false) goto error;

    // Read the FIFO to clean it
    status = i2c_.writeByte(ADXL34X_ADDRESS, ADXL34X_DATAX0_ADDR);
    if (status == false) goto error;
    status = i2c_.readByte(ADXL34X_ADDRESS, buffer, 6);
    if (status == false) goto error;

    // Write the bandwidth register
    config[0] = ADXL34X_BW_RATE_ADDR;
    config[1] = (ADXL34X_BW_RATE_RATE(13));
    status = i2c_.writeByte(ADXL34X_ADDRESS, config, sizeof(config));
    if (status == false) goto error;

    // Write the FIFO control register
    config[0] = ADXL34X_FIFO_CTL_ADDR;
    config[1] = (0x86);
    status = i2c_.writeByte(ADXL34X_ADDRESS, config, sizeof(config));
    if (status == false) goto error;

    // Write the format register
    config[0] = ADXL34X_DATA_FORMAT_ADDR;
    config[1] = (ADXL34X_DATA_FORMAT_RANGE_PM_2g);
    status = i2c_.writeByte(ADXL34X_ADDRESS, config, sizeof(config));
    if (status == false) goto error;

    // Write the bandwidth register
    config[0] = ADXL34X_INT_ENABLE_ADDR;
    config[1] = (ADXL34X_INT_ENABLE_WATERMARK);
    status = i2c_.writeByte(ADXL34X_ADDRESS, config, sizeof(config));
    if (status == false) goto error;

    // Write the power register
    config[0] = ADXL34X_POWER_CTL_ADDR;
    config[1] = ADXL34X_POWER_CTL_MEASURE;
    status = i2c_.writeByte(ADXL34X_ADDRESS, config, sizeof(config));
    if (status == false) goto error;

    // Release access to I2C
    i2c_.unlock();

    return true;

error:
    // Release access to I2C
    i2c_.unlock();
    return false;
}

bool Adxl34x::suspend(void)
{
    uint8_t config[2];
    bool status;

    // Lock access to I2C
    i2c_.lock();

    // Read the power control register
    status = i2c_.readByte(ADXL34X_POWER_CTL_ADDR, &config[1]);
    if (status == false) goto error;

    // Write the power control register (clear measure bit and set sleep bit)
    config[0]  = ADXL34X_POWER_CTL_ADDR;
    config[1] &= ~ADXL34X_POWER_CTL_MEASURE;
    config[1] |= ADXL34X_POWER_CTL_SLEEP;
    status = i2c_.writeByte(ADXL34X_ADDRESS, config, sizeof(config));
    if (status == false) goto error;

    // Release access to I2C
    i2c_.unlock();

    return true;

error:
    // Release access to I2C
    i2c_.unlock();
    return false;
}

bool Adxl34x::wakeup(void)
{
    uint8_t config[2];
    bool status;

    // Lock access to I2C
    i2c_.lock();

    // Read the power control register
    status = i2c_.readByte(ADXL34X_POWER_CTL_ADDR, &config[1]);
    if (status == false) goto error;

    // Write the power control register (clear sleep bit)
    config[0]  = ADXL34X_POWER_CTL_ADDR;
    config[1] &= ~ADXL34X_POWER_CTL_SLEEP;
    status = i2c_.writeByte(ADXL34X_ADDRESS, config, sizeof(config));
    if (status == false) goto error;

    // Write the power control register (set measure bit)
    config[0]  = ADXL34X_POWER_CTL_ADDR;
    config[1] |= ADXL34X_POWER_CTL_MEASURE;
    status = i2c_.writeByte(ADXL34X_ADDRESS, config, sizeof(config));
    if (status == false) goto error;

    // Release access to I2C
    i2c_.unlock();

    return true;

error:
    // Release access to I2C
    i2c_.unlock();
    return false;
}

bool Adxl34x::reset(void)
{
    return false;
}

bool Adxl34x::isPresent(void)
{
    uint8_t isPresent;
    bool status;

    // Lock access to I2C
    i2c_.lock();

    // Write the device identification address
    status = i2c_.writeByte(ADXL34X_ADDRESS, ADXL34X_DEVID_ADDR);
    if (status == false) goto error;

    // Read the device identification value
    status = i2c_.readByte(ADXL34X_ADDRESS, &isPresent);
    if (status == false) goto error;

    // Release access to I2C
    i2c_.unlock();

    // Return true if sensor is present
    return (isPresent == ADXL34X_DEVID_VALUE);

error:
    // Release access to I2C
    i2c_.unlock();
    return false;
}

bool Adxl34x::selfTest(bool test)
{
    uint8_t config[2];
    bool status;

    // Lock access to I2C
    i2c_.lock();

    // Read the data format register
    status = i2c_.readByte(ADXL34X_DATA_FORMAT_ADDR, &config[1]);
    if (status == false) goto error;

    // Write the data format register
    config[0]  = ADXL34X_DATA_FORMAT_ADDR;
    if (test) config[1] |= ADXL34X_DATA_FORMAT_SELF_TEST;
    else      config[1] &= ~ADXL34X_DATA_FORMAT_SELF_TEST;

    status = i2c_.writeByte(ADXL34X_ADDRESS, config, sizeof(config));
    if (status == false) goto error;

    return true;

error:
    // Release access to I2C
    i2c_.unlock();
    return false;
}

void Adxl34x::calibrate(void)
{
    int32_t accum_x = 0;
    int32_t accum_y = 0;
    int32_t accum_z = 0;
    uint8_t config[2];
    int8_t offset;

	// Lock access to I2C
    i2c_.lock();

    config[0] = ADXL34X_OFSX_ADDR;
    config[1] = 0;
    i2c_.writeByte(ADXL34X_ADDRESS, config, sizeof(config));

    config[0] = ADXL34X_OFSY_ADDR;
    config[1] = 0;
    i2c_.writeByte(ADXL34X_ADDRESS, config, sizeof(config));

    config[0] = ADXL34X_OFSZ_ADDR;
    config[1] = 0;
    i2c_.writeByte(ADXL34X_ADDRESS, config, sizeof(config));

	// Release access to I2C
    i2c_.unlock();

    for (uint16_t i = 0; i < 100; i++) {
        uint16_t x, y, z;

        readSample(&x, &y, &z);
        accum_x += x;
        accum_y += y;
        accum_z += z;
    }

    // Lock access to I2C
    i2c_.lock();

    offset    = (accum_x) / 100;
    config[0] = ADXL34X_OFSX_ADDR;
    config[1] = -(64 * offset / 256);
    i2c_.writeByte(ADXL34X_ADDRESS, config, sizeof(config));

    offset    = (accum_y) / 100;
    config[0] = ADXL34X_OFSY_ADDR;
    config[1] = -(64 * offset / 256);
    i2c_.writeByte(ADXL34X_ADDRESS, config, sizeof(config));

    offset    = (accum_z) / 100;
    config[0] = ADXL34X_OFSZ_ADDR;
    config[1] = -(64 * offset / 256);
    i2c_.writeByte(ADXL34X_ADDRESS, config, sizeof(config));

    // Release access to I2C
    i2c_.unlock();
}

void Adxl34x::setCallback(Callback* callback)
{
    gpio_.setCallback(callback);
    gpio_.enableInterrupts();
}

void Adxl34x::clearCallback(void)
{
    gpio_.clearCallback();
    gpio_.disableInterrupts();
}

bool Adxl34x::readSample(uint16_t* x, uint16_t* y, uint16_t* z)
{
    uint16_t acceleration[3];
    uint8_t  address[6] = {ADXL34X_DATAX0_ADDR, ADXL34X_DATAX1_ADDR,
                           ADXL34X_DATAY0_ADDR, ADXL34X_DATAY1_ADDR,
                           ADXL34X_DATAZ0_ADDR, ADXL34X_DATAZ1_ADDR};
    uint8_t scratch[2];
    bool status;

    // Iterate for all addresses, each direction has two addresses
    for (uint8_t i = 0; i < sizeof(address); i += 2)
    {
        // Lock access to I2C
        i2c_.lock();

        // I2C write register address
        status = i2c_.writeByte(ADXL34X_ADDRESS, address[i + 0]);
        if (status == false) goto error;

        // I2C read acceleration value
        status = i2c_.readByte(ADXL34X_ADDRESS, &scratch[0]);
        if (status == false) goto error;

        // I2C write register address
        status = i2c_.writeByte(ADXL34X_ADDRESS, address[i + 1]);
        if (status == false) goto error;

        // I2C read acceleration value
        status = i2c_.readByte(ADXL34X_ADDRESS, &scratch[1]);
        if (status == false) goto error;

        // Release access to I2C
        i2c_.unlock();

        // Convert ADXL34X data
        acceleration[i>>1] = (scratch[1] << 8) | scratch[0];
    }

    // Update acceleration variables
    *x = acceleration[0];
    *y = acceleration[1];
    *z = acceleration[2];

    return true;

error:
    // Release access to I2C
    i2c_.unlock();
    return false;
}

bool Adxl34x::readSamples(uint8_t* buffer, uint8_t samples)
{
    bool status;

    // Lock access to I2C
    i2c_.lock();

    // Read the data format register
    status = i2c_.writeByte(ADXL34X_ADDRESS, ADXL34X_DATAX0_ADDR);
    if (status == false) goto error;

    // Read the number of samples
    status = i2c_.readByte(ADXL34X_ADDRESS, buffer, samples);
    if (status == false) goto error;

    // Release access to I2C
    i2c_.unlock();

    return true;

error:
    // Release access to I2C
    i2c_.unlock();
    return false;
}

uint8_t Adxl34x::samplesAvailable()
{
    uint8_t samples;
    bool status;

    // Lock access to I2C
    i2c_.lock();

    // Read the data format register
    status = i2c_.writeByte(ADXL34X_ADDRESS, ADXL34X_FIFO_STATUS_ADDR);
    if (status == false) goto error;

    // Read the data format register
    status = i2c_.readByte(ADXL34X_ADDRESS, &samples);
    if (status == false) goto error;

    // Unlock access to I2C
    i2c_.unlock();

    return (samples & 0x3F);

error:
    // Release access to I2C
    i2c_.unlock();
    return 0;
}

/*=============================== protected =================================*/

/*================================ private ==================================*/
