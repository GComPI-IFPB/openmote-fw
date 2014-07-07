/*
 * Copyright 2013 OpenMote Technologies, S.L.
 */

/**
 *
 * @file       Gpio.h
 * @author     Pere Tuset-Peiro (peretuset@openmote.com)
 * @version    v0.1
 * @date       May, 2014
 * @brief
 * @ingroup
 *
 */

#ifndef GPIO_H_
#define GPIO_H_

#include <stdint.h>

class Gpio
{

public:
    Gpio(uint32_t port_, uint8_t pin_);
    uint32_t getPort(void);
    uint8_t getPin(void);
protected:
    uint32_t port;
    uint8_t pin;
private:

};

#endif /* GPIO_H_ */
