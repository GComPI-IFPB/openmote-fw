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

/* Board */
extern BoardImplementation board;

/* LEDs */
extern GpioOut led_green;
extern GpioOut led_orange;
extern GpioOut led_red;
extern GpioOut led_yellow;

/* User button */
extern GpioInPow button_user;

/* Debug pins */
extern GpioOut debug0;
extern GpioOut debug1;
extern GpioOut debug2;
extern GpioOut debug3;

/* SleepTimer */
extern SleepTimer sleepTimer;

/* RadioTimer */
extern RadioTimer radioTimer;

/* Watchdog */
extern Watchdog watchdog;

/* I2C */
extern I2c i2c;

/* UART */
extern Uart uart;

/* SPI */
extern Spi spi;

/* Random Number Generator */
extern RandomNumberGenerator rng;

/* IEEE 802.15.4 radio */
extern Radio radio;

/* AES module */
extern Aes aes;

/* CC2538 temperature sensor */
extern TemperatureSensor temp;

/* AT86RF215 radio transceiver */
extern At86rf215 at86rf215;

/*=============================== prototypes ================================*/

/*================================= public ==================================*/

/*================================ private ==================================*/

#endif /* BOARD_IMPLEMENTATION_HPP_ */
