/**
 * @file       BoardImplementation.hpp
 * @author     Pere Tuset-Peiro (peretuset@openmote.com)
 * @version    v0.1
 * @date       November, 2018
 * @brief
 *
 * @copyright  Copyright 2018, OpenMote Technologies, S.L.
 *             This file is licensed under the GNU General Public License v2.
 */

#ifndef BOARD_IMPLEMENTATION_HPP_
#define BOARD_IMPLEMENTATION_HPP_

/*================================ include ==================================*/

#include "Board.hpp"

#include "Aes.hpp"
#include "Gpio.hpp"
#include "I2c.hpp"
#include "Radio.hpp"
#include "RadioTimer.hpp"
#include "RandomNumberGenerator.hpp"
#include "SleepTimer.hpp"
#include "Spi.hpp"
#include "Uart.hpp"
#include "Watchdog.hpp"

#include "TemperatureSensor.hpp"

#include "at86rf215/At86rf215.hpp"

/*================================ define ===================================*/

/*================================ typedef ==================================*/

/*=============================== variables =================================*/

/* Board management */
extern BoardImplementation board;

/* LEDs */
extern GpioOut led_green;
extern GpioOut led_orange;
extern GpioOut led_red;
extern GpioOut led_yellow;

// extern GpioPwm led_green;
// extern GpioPwm led_orange;
// extern GpioPwm led_red;
// extern GpioPwm led_yellow;

/* Debug pins */
// extern GpioOut debug0;
// extern GpioOut debug1;
// extern GpioOut debug2;
// extern GpioOut debug3;

/* Adc */
extern GpioAdc gpio_adc; 

/* Buttons */
extern GpioInPow button_user;

/* Timers */
extern Timer timer0a;
extern Timer timer0b;
extern Timer timer1a;
extern Timer timer1b;
extern Timer timer2a;
extern Timer timer2b;
extern Timer timer3a;
extern Timer timer3b;

/* SleepTimer */
extern SleepTimer sleepTimer;

/* RadioTimer */
extern RadioTimer radioTimer;

// I2C peripheral
extern I2c i2c;

/* SPI peripheral */
extern Spi spi;

/* UART peripheral */
extern Uart uart;

/* IEEE 802.15.4 radio */
extern Radio radio;

/* AES module */
extern Aes aes;

/* Watchdog */
extern Watchdog watchdog;

/* Random Number Generator */
extern RandomNumberGenerator rng;

/* CC2538 Temperature sensor */
extern TemperatureSensor temp;

/* AT86RF215 radio transceiver */
extern GpioOut at86rf215_pwr;
extern GpioOut at86rf215_rst;
extern GpioOut at86rf215_csn;
extern GpioIn at86rf215_irq;
extern At86rf215 at86rf215;

/* Antenna switch */
extern GpioOut antenna_at86rf215;
extern GpioOut antenna_cc2538;

/*=============================== prototypes ================================*/

/*================================= public ==================================*/

/*================================ private ==================================*/

#endif /* BOARD_IMPLEMENTATION_HPP_ */
