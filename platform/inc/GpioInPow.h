/**
 * @file       GpioInPow.h
 * @author     Pere Tuset-Peiro (peretuset@openmote.com)
 * @version    v0.1
 * @date       May, 2015
 * @brief
 *
 * @copyright  Copyright 2015, OpenMote Technologies, S.L.
 *             This file is licensed under the GNU General Public License v2.
 */

#ifndef GPIO_IN_POW_H_
#define GPIO_IN_POW_H_

#include <stdint.h>

#include "GpioIn.h"

class GpioInPow : public GpioIn
{

public:
    GpioInPow(uint32_t port, uint8_t pin, uint32_t edge);
    void enableInterrupts(void);
    void disableInterrupts(void);
};

#endif /* GPIO_IN_POW_H_ */
