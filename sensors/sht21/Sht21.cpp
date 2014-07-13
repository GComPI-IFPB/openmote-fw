/*
 * Copyright 2013 OpenMote Technologies, S.L.
 */

/**
 *
 * @file       sht21.cpp
 * @author     Pere Tuset-Peiro (peretuset@openmote.com)
 * @version    v0.1
 * @date       May, 2014
 * @brief
 * @ingroup
 *
 */

/**********************************include************************************/

#include "Sht21.h"
#include "I2cDriver.h"

/**********************************defines************************************/

#define SHT21_ADDRESS                   ( 0x40 )

#define SHT21_USER_REG_READ             ( 0xE7 )
#define SHT21_USER_REG_WRITE            ( 0xE6 )
#define SHT21_USER_REG_RESERVED_BITS    ( 0x38 )

#define SHT21_RESOLUTION_12b_14b        ( (0 << 7) | (0 << 0) ) // 29 + 85 = 114 ms
#define SHT21_RESOLUTION_8b_12b         ( (0 << 7) | (1 << 0) ) //  4 + 22 =  26 ms
#define SHT21_RESOLUTION_10b_13b        ( (1 << 7) | (0 << 0) ) //  9 + 43 =  52 ms
#define SHT21_RESOLUTION_11b_11b        ( (1 << 7) | (1 << 0) ) // 15 + 11 =  26 ms
#define SHT21_ONCHIP_HEATER_ENABLE      ( 1 << 2 )
#define SHT21_ONCHIP_HEATER_DISABLE     ( 0 << 2 )
#define SHT21_BATTERY_ABOVE_2V25        ( 0 << 6 )
#define SHT21_BATTERY_BELOW_2V25        ( 1 << 6 )
#define SHT21_OTP_RELOAD_ENABLE         ( 0 << 1 )
#define SHT21_OTP_RELOAD_DISABLE        ( 1 << 1 )

#define SHT21_TEMPERATURE_HM_CMD        ( 0xE3 )
#define SHT21_HUMIDITY_HM_CMD           ( 0xE5 )
#define SHT21_TEMPERATURE_NHM_CMD       ( 0xF3 )
#define SHT21_HUMIDITY_NHM_CMD          ( 0xF5 )
#define SHT21_RESET_CMD                 ( 0xFE )

#define SHT21_DEFAULT_CONFIG            ( SHT21_RESOLUTION_8b_12b | \
                                          SHT21_ONCHIP_HEATER_DISABLE | \
                                          SHT21_BATTERY_ABOVE_2V25 \ \
                                          SHT21_OTP_RELOAD_DISABLE )

/*********************************variables***********************************/



/**********************************public*************************************/

Sht21::Sht21(I2cDriver* i2c_):
    i2c(i2c_)
{
}

bool Sht21::enable(void)
{
    bool status;
    uint8_t config;
    
    i2c->lock();
    
    status = i2c->writeByte(SHT21_ADDRESS, SHT21_READ_USER_REG);
    status = i2c->readByte(SHT21_ADDRESS, &config);

    config &= SHT21_USER_REG_RESERVED_BITS;
    config |= SHT21_DEFAULT_CONFIG;

    status = i2c->writeByte(SHT21_ADDRESS, SHT21_WRITE_USER_REG, &config);
    
    i2c->unlock();

    return status;
}

bool Sht21::reset(void)
{
    bool status;
    
    i2c->lock();
    status = i2c->writeByte(SHT21_ADDRESS, SHT21_RESET_CMD);
    i2c->unlock();
    
    return status;
}

bool Sht21::isPresent(void)
{
    bool status;
    uint8_t isPresent;
    
    i2c->lock();
    status = i2c->writeByte(SHT21_ADDRESS, SHT21_READ_USER_REG);
    status = i2c->readByte(SHT21_ADDRESS, &isPresent);
    i2c->unlock();
    
    return (status && isPresent == SHT21_CONFIG_DEFAULT);
}

void Sht21::readTemperature(void)
{
    bool status;
    uint8_t sht21_temperature[2];
    
    i2c->lock();
    status = i2c->writeByte(SHT21_ADDRESS, SHT21_TEMPERATURE_HM_CMD);
    status = i2c->readByte(SHT21_ADDRESS, sht21_temperature, sizeof(sht21_temperature));
    i2c->unlock();

    if (status)
    {
        temperature = (sht21_temperature[1] << 8) | sht21_temperature[0];
    }
}

void Sht21::readHumidity(void)
{
    bool status;
    uint8_t sht21_humidity[2];

    i2c->lock();
    status = i2c->writeByte(SHT21_ADDRESS, SHT21_HUMIDITY_HM_CMD);
    status = i2c->readByte(SHT21_ADDRESS, sht21_humidity, sizeof(sht21_humidity));
    i2c->unlock();

    if (status)
    {
        humidity = (sht21_humidity[1] << 8) | sht21_humidity[0];
    }
}

float Sht21::getTemperature(void)
{
    float result;
    result = -46.85;
    result += 175.72 * temperature / 65536;
    return result;
}

uint16_t Sht21::getTemperatureRaw(void)
{
    return temperature;
}

uint16_t Sht21::getHumidityRaw(void)
{
    return humidity;
}

float Sht21::getHumidity(void)
{
    float result;
    result = -6.0;
    result += 125.0 * humidity / 65536;
    return result;
}

/*********************************protected***********************************/



/**********************************private************************************/

