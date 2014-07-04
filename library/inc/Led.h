/*
 * Copyright 2013 OpenMote Technologies, S.L.
 */

/**
 *
 * @file       Led.h
 * @author     Pere Tuset-Peiro (peretuset@openmote.com)
 * @version    v0.1
 * @date       May, 2014
 * @brief
 * @ingroup
 *
 */

#ifndef LED_H_
#define LED_H_

#include "Gpio.h"

class Led : public Gpio
{

public:
    Led(uint32_t port_, uint8_t pin_);
private:
    

};

#endif /* LED_H_ */
