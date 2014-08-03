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
#include "InterruptHandler.h"

class GpioAdc : public Gpio
{
public:
    GpioAdc(uint32_t port_, uint8_t pin_, uint32_t adc_);
    void init(uint32_t resolution_, uint32_t reference_);
    uint32_t read(void);
protected:
private:
    uint32_t adc;
};

#endif /* GPIO_ADC_H_ */
