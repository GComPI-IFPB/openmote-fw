/*
 * Copyright 2013 OpenMote Technologies, S.L.
 */

/**
 *
 * @file       GpioAdc.h
 * @author     Pere Tuset-Peiro (peretuset@openmote.com)
 * @version    v0.1
 * @date       May, 2014
 * @brief
 * @ingroup
 *
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
