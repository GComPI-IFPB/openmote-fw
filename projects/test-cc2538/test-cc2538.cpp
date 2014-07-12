/*
 * Copyright 2013 OpenMote Technologies, S.L.
 */

/**
 *
 * @file       test-cc2538.cpp
 * @author     Pere Tuset-Peiro (peretuset@openmote.com)
 * @version    v0.1
 * @date       May, 2014
 * @brief
 * @ingroup
 *
 */

/*================================ include ==================================*/

#include "openmote-cc2538.h"

/*================================ define ===================================*/

#define DELAY_TICKS                 ( 160000 )

/*================================ typedef ==================================*/

/*=============================== variables =================================*/

static uint8_t adxl346_string[] = "ADXL346 is present.";
static uint8_t max44009_string[] = "MAX44009 is present.";
static uint8_t sht21_string[] = "SHT21 is present.";

/*=============================== prototypes ================================*/

static void button_user_callback(void);

/*================================= public ==================================*/

static void button_user_callback(void)
{    
    if (max44009.isPresent() == true) {
        led_orange.off();
        
        max44009.readLux();
        uint16_t lux = max44009.getLuxRaw();
        
        uart.writeByte(max44009_string, sizeof(max44009_string));
        uart.writeByte((uint8_t *)&lux, sizeof(lux));
    } else {
        led_orange.on();
    }
    
    if (adxl346.isPresent() == true) {
        led_yellow.off();
        
        adxl346.readAcceleration();
        
        uint16_t x = adxl346.getX();
        uint16_t y = adxl346.getY();
        uint16_t z = adxl346.getZ();
        
        uart.writeByte(adxl346_string, sizeof(adxl346_string));
        uart.writeByte((uint8_t *)&x, sizeof(x));
        uart.writeByte((uint8_t *)&y, sizeof(y));
        uart.writeByte((uint8_t *)&z, sizeof(z));
    } else {
        led_yellow.on();
    }
    
    if (sht21.isPresent() == true) {
        led_red.off();
        
        sht21.readTemperature();
        sht21.readHumidity();
        
        uint16_t temperature = sht21.getTemperatureRaw();
        uint16_t humidity = sht21.getHumidityRaw();
        
        uart.writeByte(sht21_string, sizeof(sht21_string));
        uart.writeByte((uint8_t *)&temperature, sizeof(temperature));
        uart.writeByte((uint8_t *)&humidity, sizeof(humidity));	
    } else {
        led_red.on();
    }
}

int main(void)
{
    volatile uint32_t i = 0;
    
    led_green.on();
    led_red.on();
    led_yellow.on();
    led_orange.on();

    button_user.setCallback(button_user_callback);
    button_user.enableInterrupt();

    uart.enable(UART_BAUDRATE, UART_CONFIG, UART_INT_MODE);
    
    i2c.enable(I2C_BAUDRATE);
        
    while(true){
        led_green.toggle();
        uart.writeByte('a');
        for(i = DELAY_TICKS; i > 0; i--);
    }

    return 0;
}

/*================================ private ==================================*/

