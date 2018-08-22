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

#include "si7006/Si7006.h"
#include "at86rf215/At86rf215.h"

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

// extern GpioPwm led_green;
// extern GpioPwm led_orange;
// extern GpioPwm led_red;
// extern GpioPwm led_yellow;

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
extern Timer timer0a;
extern Timer timer0b;
extern Timer timer1a;
extern Timer timer1b;
extern Timer timer2a;
extern Timer timer2b;
extern Timer timer3a;
extern Timer timer3b;

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
extern GpioOut at86rf215_pwr;
extern GpioOut at86rf215_rst;
extern GpioOut at86rf215_csn;
extern GpioIn at86rf215_irq;
extern At86rf215 at86rf215;

extern GpioOut antenna_at86rf215;
extern GpioOut antenna_cc2538;

/*=============================== prototypes ================================*/

/*================================= public ==================================*/

/*================================ private ==================================*/

#endif /* BOARD_OPENMOTE_H_ */
