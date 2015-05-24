/**
 * @file       GpioPwm.h
 * @author     Pere Tuset-Peiro (peretuset@openmote.com)
 * @version    v0.1
 * @date       May, 2015
 * @brief
 *
 * @copyright  Copyright 2015, OpenMote Technologies, S.L.
 *             This file is licensed under the GNU General Public License v2.
 */

#ifndef GPIO_PWM_H_
#define GPIO_PWM_H_

#include <stdint.h>

#include "Gpio.h"

class GpioPwm : public Gpio
{
public:
    GpioPwm(uint32_t port, uint8_t pin);
};

#endif /* GPIO_PWM_H_ */
