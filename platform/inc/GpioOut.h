/**
 * @file       GpioOut.h
 * @author     Pere Tuset-Peiro (peretuset@openmote.com)
 * @version    v0.1
 * @date       May, 2015
 * @brief
 *
 * @copyright  Copyright 2015, OpenMote Technologies, S.L.
 *             This file is licensed under the GNU General Public License v2.
 */
#ifndef GPIO_OUT_H_
#define GPIO_OUT_H_

#include <stdint.h>

#include "Gpio.h"

class GpioOut : public Gpio
{

public:
    GpioOut(uint32_t port, uint8_t pin);
    void on(void);
    void off(void);
    void toggle(void);
    uint32_t status(void);
};

#endif /* GPIO_OUT_H_ */
