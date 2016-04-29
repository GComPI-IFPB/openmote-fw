/**
 * @file       platform_types.h
 * @author     Pere Tuset-Peiro (peretuset@openmote.com)
 * @version    v0.1
 * @date       May, 2015
 * @brief
 *
 * @copyright  Copyright 2015, OpenMote Technologies, S.L.
 *             This file is licensed under the GNU General Public License v2.
 */

#ifndef PLATFORM_TYPES_H_
#define PLATFORM_TYPES_H_

/*================================ include ==================================*/

#include "cc2538_include.h"

/*================================ define ===================================*/

/*================================ typedef ==================================*/

/**
 * SleepMode_None:   ~2 mA,   0 + 0 us,   wake-up from any interrupt source (e.g. UART)
 * SleepMode_1:    ~600 uA, 0.5 + 4 us,   wake-up from Gpio, Sleep timer, USB resume
 * SleepMode_2:    ~1.5 uA, 136 + 136 us, wake-up from Gpio, Sleep timer
 * SleepMode_3:    ~0.8 uA, 136 + 136 us, wake-up from Gpio
 */
enum SleepMode : uint8_t {
    SleepMode_None = SYS_CTRL_PM_NOACTION,
    SleepMode_1    = SYS_CTRL_PM_1,
    SleepMode_2    = SYS_CTRL_PM_2,
    SleepMode_3    = SYS_CTRL_PM_3
};

struct GpioConfig {
	uint32_t port;
	uint8_t pin;
	uint32_t ioc;
	uint32_t edge;
	uint32_t adc;
};

struct TimerConfig {
	uint32_t peripheral;
	uint32_t base;
	uint32_t source;
	uint32_t config;
	uint32_t interrupt;
	uint32_t interrupt_mode;
};

struct I2cConfig {
	uint32_t peripheral;
	uint32_t baudrate;
};

struct SpiConfig {
	uint32_t peripheral;
	uint32_t base;
	uint32_t clock;
	uint32_t interrupt;
	uint32_t mode;
	uint32_t protocol;
	uint32_t datawidth;
	uint32_t baudrate;
};

struct UartConfig {
	uint32_t peripheral;
	uint32_t base;
	uint32_t clock;
	uint32_t interrupt;
	uint32_t baudrate;
	uint32_t mode;
};

/*=============================== variables =================================*/

/*=============================== prototypes ================================*/

/*================================= public ==================================*/

/*================================ private ==================================*/

#endif  /* PLATFORM_TYPES_H_ */
