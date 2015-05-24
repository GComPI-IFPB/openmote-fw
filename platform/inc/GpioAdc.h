/**
 * @file       GpioAdc.h
 * @author     Pere Tuset-Peiro (peretuset@openmote.com)
 * @version    v0.1
 * @date       May, 2015
 * @brief
 *
 * @copyright  Copyright 2015, OpenMote Technologies, S.L.
 *             This file is licensed under the GNU General Public License v2.
 */

#ifndef GPIO_ADC_H_
#define GPIO_ADC_H_

#include <stdint.h>

#include "Gpio.h"

class GpioAdc : public Gpio
{
public:
    GpioAdc(uint32_t port, uint8_t pin, uint32_t adc);
    void init(uint32_t resolution, uint32_t reference);
    uint32_t read(void);
private:
    uint32_t adc_;
};

#endif /* GPIO_ADC_H_ */
