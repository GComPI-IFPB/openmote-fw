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

#include "Sht21.h"
#include "I2c.h"

#define SHT21_ADDRESS                   ( 0x40 )

#define SHT21_WAKEUP_TIME_US            ( 15000 )

#define SHT21_CONFIG_DEFAULT            ( 0x3A )

#define SHT21_READ_USER_REG             ( 0xE7 )
#define SHT21_WRITE_USER_REG            ( 0xE6 )

#define SHT21_TEMPERATURE_HM_CMD        ( 0xE3 )
#define SHT21_HUMIDITY_HM_CMD           ( 0xE5 )
#define SHT21_TEMPERATURE_NHM_CMD       ( 0xF3 )
#define SHT21_HUMIDITY_NHM_CMD          ( 0xF5 )
#define SHT21_RESET_CMD                 ( 0xFE )

Sht21::Sht21(I2c* i2c_):
    i2c(i2c_)
{
}

void Sht21::enable(void)
{
}

void Sht21::reset(void)
{
    i2c->lock();
    i2c->writeByte(SHT21_ADDRESS, SHT21_RESET_CMD);
    i2c->unlock();
}

bool Sht21::isPresent(void)
{
    uint8_t status;
    
    i2c->lock();
    status = i2c->readByte(SHT21_ADDRESS, SHT21_READ_USER_REG);
    i2c->unlock();
    
    return (status == SHT21_CONFIG_DEFAULT);
}

void Sht21::readTemperature(void)
{
    uint8_t sht21_temperature[2];

    i2c->lock();
    i2c->readByte(SHT21_ADDRESS, SHT21_TEMPERATURE_HM_CMD, sht21_temperature, sizeof(sht21_temperature));
    i2c->unlock();

    temperature = (sht21_temperature[1] << 8) | sht21_temperature[0];
}

void Sht21::readHumidity(void)
{
    uint8_t sht21_humidity[2];

    i2c->lock();
    i2c->readByte(SHT21_ADDRESS, SHT21_HUMIDITY_HM_CMD, sht21_humidity, sizeof(sht21_humidity));
    i2c->unlock();

    humidity = (sht21_humidity[1] << 8) | sht21_humidity[0];
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
