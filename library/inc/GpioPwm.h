/*
 * Copyright 2013 OpenMote Technologies, S.L.
 */

/**
 *
 * @file       GpioPwm.h
 * @author     Pere Tuset-Peiro (peretuset@openmote.com)
 * @version    v0.1
 * @date       May, 2014
 * @brief
 * @ingroup
 *
 */

#ifndef GPIO_PWM_H_
#define GPIO_PWM_H_

#include <stdint.h>

#include "Gpio.h"

class GpioPwm : public Gpio
{
public:
    GpioPwm(uint32_t port_, uint8_t pin_);
};

#endif /* GPIO_PWM_H_ */
