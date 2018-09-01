/**
 * @file       Opt3001.cpp
 * @author     Pere Tuset-Peiro (peretuset@openmote.com)
 * @version    v0.1
 * @date       May, 2015
 * @brief
 *
 * @copyright  Copyright 2015, OpenMote Technologies, S.L.
 *             This file is licensed under the GNU General Public License v2.
 */

/*================================ include ==================================*/

#include <math.h>

#include "Task.hpp"

#include "Opt3001.hpp"

#include "I2c.hpp"

/*================================ define ===================================*/

/* Register addresses */
#define REG_RESULT                      ( 0x00 )
#define REG_CONFIGURATION               ( 0x01 )
#define REG_LOW_LIMIT                   ( 0x02 )
#define REG_HIGH_LIMIT                  ( 0x03 )

#define REG_MANUFACTURER_ID             ( 0x7E )
#define REG_DEVICE_ID                   ( 0x7F )

/* Register values */
#define MANUFACTURER_ID                 ( 0x5449 )
#define DEVICE_ID                       ( 0x3001 )
#define CONFIG_RESET                    ( 0xC810 )
#define CONFIG_TEST                     ( 0xCC10 )
#define CONFIG_ENABLE                   ( 0x10C4 )
#define CONFIG_DISABLE                  ( 0x10C0 )

/* Bit values */
#define DATA_RDY_BIT                    ( 0x0080 )

/* Register length */
#define REGISTER_LENGTH                 ( 2 )

/* Sensor data size */
#define DATA_LENGTH                     ( 2 )

/*================================ typedef ==================================*/

/*=============================== variables =================================*/

/*=============================== prototypes ================================*/

/*================================= public ==================================*/

Opt3001::Opt3001(I2c& i2c, uint8_t address):
    i2c_(i2c), address_(address)
{
}

bool Opt3001::init(void)
{

}

bool Opt3001::enable(void)
{
    uint16_t val = CONFIG_ENABLE;

    /* Obtain the mutex of the I2C driver */
    i2c_.lock();

    // i2c_write(OPT3001_I2C_ADDRESS, REG_CONFIGURATION, (uint8_t*)&val, REGISTER_LENGTH);
    if (false)
    {
        goto error;
    }

    /* Release the mutex of the I2C driver */
    i2c_.unlock();

    return true;
error:
    /* Release the mutex of the I2C driver */
    i2c_.unlock();
    return false;
}

bool Opt3001::disable(void)
{
    uint16_t val = CONFIG_DISABLE;

    /* Obtain the mutex of the I2C driver */
    i2c_.lock();

    // i2c_write(address_, REG_CONFIGURATION, (uint8_t*)&val, REGISTER_LENGTH);
    if (false)
    {
        goto error;
    }

    /* Release the mutex of the I2C driver */
    i2c_.unlock();

    return true;
error:
    /* Release the mutex of the I2C driver */
    i2c_.unlock();
    return false;
}

bool Opt3001::read(uint16_t* raw)
{
    int8_t success;
    uint16_t val;

    /* Obtain the mutex of the I2C driver */
    i2c_.lock();

    // success = i2c_read(address_, REG_CONFIGURATION, (uint8_t *)&val, REGISTER_LENGTH);
    if (success < 0)
    {
        goto error;
    }

    success = (val & DATA_RDY_BIT) == DATA_RDY_BIT;
    if (success)
    {
        // success = i2c_read(address_, REG_RESULT, (uint8_t *)&val, REGISTER_LENGTH);
    }

    if (success < 0)
    {
        goto error;
    }

    // Swap bytes
    *raw = (val << 8) | (val >> 8 & 0xFF);

    /* Release the mutex of the I2C driver */
    i2c_.unlock();

    return true;

error:
    /* Release the mutex of the I2C driver */
    i2c_.unlock();
    return false;
}

void Opt3001::convert(uint16_t raw, float* lux)
{
    uint16_t e, m;

    m = raw & 0x0FFF;
    e = (raw & 0xF000) >> 12;

    *lux = m * (0.01 * exp2(e));
}

bool Opt3001::test(void)
{
    uint8_t scratch[3];
    uint16_t val;

    /* Obtain the mutex of the I2C driver */
    i2c_.lock();

    /* Prepare I2C buffer */
    scratch[0] = REG_MANUFACTURER_ID;
    scratch[1] = 0x00;
    scratch[2] = 0x00;

    /* Perform I2C read transaction */
    i2c_.readByte(address_, scratch, 3);

    /* Check manufacturer ID */
    val = (scratch[1] << 8) | (scratch[2] << 0);
    if (val != MANUFACTURER_ID)
    {
        goto error;
    }

    /* Prepare I2C buffer */
    scratch[0] = REG_DEVICE_ID;
    scratch[1] = 0x00;
    scratch[2] = 0x00;

    /* Perform I2C read transaction */
    i2c_.readByte(address_, scratch, 3);

    /* Check device ID */
    val = (scratch[1] << 8) | (scratch[2] << 0);
    if (val != DEVICE_ID)
    {
        goto error;
    }

    /* Release the mutex of the I2C driver */
    i2c_.unlock();

    return true;

error:
    /* Release the mutex of the I2C driver */
    i2c_.unlock();
    return false;
}

/*=============================== protected =================================*/

/*================================ private ==================================*/

