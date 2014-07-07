/*
 * Copyright 2013 OpenMote Technologies, S.L.
 */

/**
 *
 * @file       GpioOut.h
 * @author     Pere Tuset-Peiro (peretuset@openmote.com)
 * @version    v0.1
 * @date       May, 2014
 * @brief
 * @ingroup
 *
 */

#ifndef GPIO_OUT_H_
#define GPIO_OUT_H_

#include <stdint.h>

#include "Gpio.h"

class GpioOut : public Gpio
{

public:
    GpioOut(uint32_t port_, uint8_t pin_);
    void on(void);
    void off(void);
    void toggle(void);
    uint32_t status(void);
private:
    
};

#endif /* GPIO_OUT_H_ */
