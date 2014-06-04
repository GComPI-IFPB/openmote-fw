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

#define SHT21_I2C_ADDRESS               ( 0x40 ) /* SHT21 I2C address, last bit is read/write */

#define SHT21_WAKEUP_TIME_US            ( 15000 )

#define SHT21_READ_USER_REG             ( 0xE7 )
#define SHT21_WRITE_USER_REG            ( 0xE7 )

#define SHT21_TEMPERATURE_HM_CMD        ( 0xE3 )
#define SHT21_HUMIDITY_HM_CMD           ( 0xE5 )
#define SHT21_TEMPERATURE_NHM_CMD       ( 0xF3 )
#define SHT21_HUMIDITY_NHM_CMD          ( 0xF5 )
#define SHT21_RESET_CMD                 ( 0xFE )

Sht21::Sht21(I2c* i2c_):
    i2c(i2c_)
{
    // board_delay_us(SHT21_WAKEUP_TIME_US);

    // i2c_enable();

    // sht21_status = i2c_read_single(SHT21_I2C_ADDRESS, SHT21_READ_USER_REG);
    // sht21_status = sht21_status & 0x3A;

    // i2c_write_single(SHT21_I2C_ADDRESS, SHT21_WRITE_USER_REG, sht21_status);
}

void Sht21::reset(void)
{
    // i2c_enable();
    // i2c_command(SHT21_I2C_ADDRESS, SHT21_RESET_CMD);

    // board_delay_us(SHT21_WAKEUP_TIME_US);
}

void Sht21::readTemperature(void)
{
    // uint8_t sht21_humidity[2];

    // i2c_enable();
    // i2c_read_multiple(SHT21_I2C_ADDRESS, SHT21_HUMIDITY_HM_CMD, sht21_humidity, sizeof(sht21_humidity));

    // sht21_data->humidity_raw = (sht21_humidity[1] << 8) | sht21_humidity[0];
}

void Sht21::readHumidity(void)
{
    // uint8_t sht21_temperature[2];

    // i2c_enable();
    // i2c_read_multiple(SHT21_I2C_ADDRESS, SHT21_TEMPERATURE_HM_CMD, sht21_temperature, sizeof(sht21_temperature));

    // sht21_data->temperature_raw = (sht21_temperature[1] << 8) | sht21_temperature[0];
}

float Sht21::getTemperature(void)
{
    float result;
    result = -46.85;
    result += 175.72 * temperature / 65536;
    return result;
}

float Sht21::getHumidity(void)
{
    float result;
    result = -6.0;
    result += 125.0 * humidity / 65536;
    return result;
}
