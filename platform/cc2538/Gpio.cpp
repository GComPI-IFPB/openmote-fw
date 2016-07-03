/**
 * @file       Gpio.cpp
 * @author     Pere Tuset-Peiro (peretuset@openmote.com)
 * @version    v0.1
 * @date       May, 2015
 * @brief
 *
 * @copyright  Copyright 2015, OpenMote Technologies, S.L.
 *             This file is licensed under the GNU General Public License v2.
 */

/*================================ include ==================================*/

#include "Gpio.h"

#include "cc2538_include.h"
#include "platform_types.h"

/*================================ define ===================================*/

/*================================ typedef ==================================*/

/*=============================== variables =================================*/

/*=============================== prototypes ================================*/

/*================================= public ==================================*/

Gpio::Gpio(GpioConfig& config) : config_(config)
{
}

bool Gpio::operator==(const Gpio& other) {
	return (config_ == other.config_);
}

GpioConfig& Gpio::getGpioConfig(void)
{
	return config_;
}

/*=============================== protected =================================*/

/*================================ private ==================================*/
