/**
 * @file       Si7006.cpp
 * @author     Pere Tuset-Peiro (peretuset@openmote.com)
 * @version    v0.1
 * @date       May, 2015
 * @brief
 *
 * @copyright  Copyright 2015, OpenMote Technologies, S.L.
 *             This file is licensed under the GNU General Public License v2.
 */

/*================================ include ==================================*/

#include "Si7006.hpp"
#include "Scheduler.hpp"

#include "platform_types.h"

/*================================ define ===================================*/

#define SI7006_ADDRESS                   ( 0x40 )
#define SI7006_DELAY_MS                  ( 20 )

#define SI7006_USER_REG_READ             ( 0xE7 )
#define SI7006_USER_REG_WRITE            ( 0xE6 )
#define SI7006_USER_REG_RESERVED_BITS    ( 0x38 )

#define SI7006_RESOLUTION_12b_14b        ( (0 << 7) | (0 << 0) )
#define SI7006_RESOLUTION_8b_12b         ( (0 << 7) | (1 << 0) )
#define SI7006_RESOLUTION_10b_13b        ( (1 << 7) | (0 << 0) )
#define SI7006_RESOLUTION_11b_11b        ( (1 << 7) | (1 << 0) )
#define SI7006_BATTERY_ABOVE_2V25        ( 0 << 6 )
#define SI7006_BATTERY_BELOW_2V25        ( 1 << 6 )
#define SI7006_ONCHIP_HEATER_ENABLE      ( 1 << 2 )
#define SI7006_ONCHIP_HEATER_DISABLE     ( 0 << 2 )
#define SI7006_OTP_RELOAD_ENABLE         ( 0 << 1 )
#define SI7006_OTP_RELOAD_DISABLE        ( 1 << 1 )

#define SI7006_TEMPERATURE_HM_CMD        ( 0xE3 )
#define SI7006_HUMIDITY_HM_CMD           ( 0xE5 )
#define SI7006_TEMPERATURE_NHM_CMD       ( 0xF3 )
#define SI7006_HUMIDITY_NHM_CMD          ( 0xF5 )
#define SI7006_RESET_CMD                 ( 0xFE )

#define SI7006_STATUS_MASK               ( 0xFC )

#define SI7006_DEFAULT_CONFIG            ( SI7006_RESOLUTION_12b_14b | \
                                          SI7006_ONCHIP_HEATER_DISABLE | \
                                          SI7006_BATTERY_ABOVE_2V25 | \
                                          SI7006_OTP_RELOAD_DISABLE )

#define SI7006_USER_CONFIG               ( SI7006_RESOLUTION_8b_12b | \
                                          SI7006_ONCHIP_HEATER_DISABLE | \
                                          SI7006_BATTERY_ABOVE_2V25 | \
                                          SI7006_OTP_RELOAD_DISABLE )

/*================================ typedef ==================================*/

/*=============================== variables =================================*/

/*=============================== prototypes ================================*/

/*================================= public ==================================*/

Si7006::Si7006(I2c& i2c):
    i2c_(i2c)
{
}

bool Si7006::enable(void)
{
    uint8_t config[2];
    bool status;

    // Check if the sensor has been initialized
    isInitialized();

    // Setup the configuration vector, the first position holds address
    // and the second position holds the actual configuration
    config[0] = SI7006_USER_REG_WRITE;
    config[1] = 0;

    // Obtain the mutex of the I2C driver
    i2c_.lock();

    // Write the configuration register address
    status = i2c_.writeByte(SI7006_ADDRESS, SI7006_USER_REG_READ);
    if (!status) goto error;

    // Read the current configuration (see datasheet pag. 9, fig. 18)
    status = i2c_.readByte(SI7006_ADDRESS, &config[1]);
    if (!status) goto error;

    // Clean all the configuration bits except those reserved
    config[1] &= SI7006_USER_REG_RESERVED_BITS;

    // Set the configuration bits without changing those reserved
    config[1] |= SI7006_USER_CONFIG;

    // Write the user configuration
    status = i2c_.writeByte(SI7006_ADDRESS, config, sizeof(config));
    if (!status) goto error;

    // Release the mutex of the I2C driver
    i2c_.unlock();

    return true;

error:
    // Release the mutex of the I2C driver
    i2c_.unlock();
    return false;
}

bool Si7006::reset(void)
{
    bool status;

    // Obtain the mutex of the I2C driver
    i2c_.lock();

    // Send a soft-reset command (see datasheet pag. 9, fig. 17)
    status = i2c_.writeByte(SI7006_ADDRESS, SI7006_RESET_CMD);
    if (!status) goto error;

    // Release the mutex of the I2C driver
    i2c_.unlock();

    return true;

error:
    // Release the mutex of the I2C driver
    i2c_.unlock();
    return false;
}

bool Si7006::isPresent(void)
{
    uint8_t isPresent;
    bool status;

    // Check if the sensor has been initialized
    isInitialized();

    // Obtain the mutex of the I2C driver
    i2c_.lock();

    //  Write the configuration register address
    status = i2c_.writeByte(SI7006_ADDRESS, SI7006_USER_REG_READ);
    if (!status) goto error;

    // Read the current configuration
    status = i2c_.readByte(SI7006_ADDRESS, &isPresent);
    if (!status) goto error;

    // Clear the reserved bits (see datasheet pag. 9, tab. 8)
    isPresent &= ~SI7006_USER_REG_RESERVED_BITS;

    // Release the mutex of the I2C driver
    i2c_.unlock();

    // Return true if sensor is present
    return (isPresent == SI7006_DEFAULT_CONFIG ||
            isPresent == SI7006_USER_CONFIG);

error:
    // Release the mutex of the I2C driver
    i2c_.unlock();
    return false;
}

bool Si7006::read(void)
{
    bool status;

    status = readTemperature();
    if (!status) return false;

    status = readHumidity();
    if (!status) return false;

    return true;
}

bool Si7006::readTemperature(void)
{
    uint8_t si7006_temperature[2];
    bool status;

    // Obtain the mutex of the I2C driver
    i2c_.lock();

    // Write the read temperature command
    status = i2c_.writeByte(SI7006_ADDRESS, SI7006_TEMPERATURE_HM_CMD);
    if (!status) goto error;

    // Read the current temperature (see datasheet pag. 8, fig. 15)
    status = i2c_.readByte(SI7006_ADDRESS, si7006_temperature, sizeof(si7006_temperature));
    if (!status) goto error;

    // Release the mutex of the I2C driver
    i2c_.unlock();

    // Update the temperature
    temperature = (si7006_temperature[0] << 8) | (si7006_temperature[1] & SI7006_STATUS_MASK);

    return true;

error:
    // Release the mutex of the I2C driver
    i2c_.unlock();
    return false;
}

bool Si7006::readHumidity(void)
{
    uint8_t si7006_humidity[2];
    bool status;

    // Obtain the mutex of the I2C driver
    i2c_.lock();

    // Write the read humidity command
    status = i2c_.writeByte(SI7006_ADDRESS, SI7006_HUMIDITY_HM_CMD);
    if (!status) goto error;

    // Read the current humidity (see datasheet  pag. 8, fig. 15)
    status = i2c_.readByte(SI7006_ADDRESS, si7006_humidity, sizeof(si7006_humidity));
    if (!status) goto error;

    // Release the mutex of the I2C driver
    i2c_.unlock();

    // Update the humidity
    humidity = (si7006_humidity[0] << 8) | (si7006_humidity[1] & SI7006_STATUS_MASK);

    return true;

error:
    // Release the mutex of the I2C driver
    i2c_.unlock();
    return false;
}

float Si7006::getTemperature(void)
{
    float result;
    result = -46.85;
    result += 175.72 * temperature / 65536;
    return result;
}

uint16_t Si7006::getTemperatureRaw(void)
{
    return temperature;
}

uint16_t Si7006::getHumidityRaw(void)
{
    return humidity;
}

float Si7006::getHumidity(void)
{
    float result;
    result = -6.0;
    result += 125.0 * humidity / 65536;
    return result;
}

/*=============================== protected =================================*/

/*================================ private ==================================*/

void Si7006::isInitialized(void)
{
    static bool isInitialized = false;

    // If sensor is not initialized
    if (!isInitialized)
    {
        // Wait until sensor is available
        Scheduler::delay_ms(SI7006_DELAY_MS);

        // The sensor is now initialized
        isInitialized = true;
    }
}
