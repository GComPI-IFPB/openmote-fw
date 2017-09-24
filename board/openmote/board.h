/**
 * @file       board.h
 * @author     Pere Tuset-Peiro (peretuset@openmote.com)
 * @version    v0.1
 * @date       May, 2015
 * @brief
 *
 * @copyright  Copyright 2015, OpenMote Technologies, S.L.
 *             This file is licensed under the GNU General Public License v2.
 */

#ifndef BOARD_OPENMOTE_H_
#define BOARD_OPENMOTE_H_

/*================================ include ==================================*/

#include "Aes.h"
#include "Board.h"
#include "Gpio.h"
#include "I2c.h"
#include "Radio.h"
#include "RadioTimer.h"
#include "SleepTimer.h"
#include "Spi.h"
#include "TemperatureSensor.h"
#include "Timer.h"
#include "Uart.h"
#include "RandomNumberGenerator.h"
#include "Watchdog.h"

#include "Si7006.h"
#include "At86rf215.h"

/*================================ define ===================================*/

/*================================ typedef ==================================*/

/*=============================== variables =================================*/

// Board management
extern Board board;
extern Watchdog watchdog;
extern RandomNumberGenerator rng;

// Leds
extern GpioOut led_green;
extern GpioOut led_orange;
extern GpioOut led_red;
extern GpioOut led_yellow;

// Debug
extern GpioOut debug0;
extern GpioOut debug1;
extern GpioOut debug2;
extern GpioOut debug3;

// Adc
extern GpioAdc gpio_adc; 

// Button
extern GpioInPow button_user;

// Timer
extern Timer timer0;
extern Timer timer1;
extern Timer timer2;
extern Timer timer3;

// SleepTimer
extern SleepTimer sleepTimer;

// RadioTimer
extern RadioTimer radioTimer;

// I2C peripheral
extern I2c i2c;

// SPI peripheral
extern Spi spi;

// UART peripheral
extern Uart uart;

// IEEE 802.15.4 radio
extern Radio radio;

// AES module
extern Aes aes;

// CC2538 Temperature sensor
extern TemperatureSensor temp;

// Temperature + Relative humidity sensor
extern Si7006 si7006;

// AT86RF215 radio transceiver
extern At86rf215 at86rf215;

/*=============================== prototypes ================================*/

/*================================= public ==================================*/

/*================================ private ==================================*/

#endif /* BOARD_OPENMOTE_H_ */