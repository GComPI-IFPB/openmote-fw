/**
 * @file       openmote-cc2538.h
 * @author     Pere Tuset-Peiro (peretuset@openmote.com)
 * @version    v0.1
 * @date       May, 2015
 * @brief
 *
 * @copyright  Copyright 2015, OpenMote Technologies, S.L.
 *             This file is licensed under the GNU General Public License v2.
 */

#ifndef BOARD_OPENMOTE_CC2538_H_
#define BOARD_OPENMOTE_CC2538_H_

/*================================ include ==================================*/

/*================================ define ===================================*/

/*================================ typedef ==================================*/

class Aes;
class Board;
class GpioIn;
class GpioInPow;
class GpioOut;
class I2c;
class Radio;
class RadioTimer;
class SleepTimer;
class Spi;
class TemperatureSensor;
class Timer;
class Uart;
class RandomNumberGenerator;
class Watchdog;

class Adxl34x;
class Max44009;
class Sht21;
class Si7006;

class Tps62730;
class Cc1200;

/*=============================== variables =================================*/

// Board management
extern Board board;
extern Watchdog watchdog;
extern RandomNumberGenerator rng;

// Step-down DC/DC converter
extern Tps62730 tps62730;

// Debug pins
extern GpioOut debug_ad0;
extern GpioOut debug_ad1;
extern GpioOut debug_ad2;

// Leds
extern GpioOut led_green;
extern GpioOut led_orange;
extern GpioOut led_red;
extern GpioOut led_yellow;

// Button
extern GpioInPow button_user;

// Antenna
extern GpioOut antenna_external;
extern GpioOut antenna_internal;

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

// Acceleration sensor
extern Adxl34x adxl34x;

// Light sensor
extern Max44009 max44009;

// Temperature + Relative humidity sensor
// extern Sht21 sht21;
extern Si7006 si7006;

// CC1200 transceiver
extern Cc1200 cc1200;
extern GpioIn cc1200_gpio0;
extern GpioIn cc1200_gpio2;
extern GpioIn cc1200_gpio3;

/*=============================== prototypes ================================*/

/*================================= public ==================================*/

/*================================ private ==================================*/

#endif /* BOARD_OPENMOTE_CC2538_H_ */
