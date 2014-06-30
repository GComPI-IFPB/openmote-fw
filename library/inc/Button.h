/*
 * Copyright 2013 OpenMote Technologies, S.L.
 */

/**
 *
 * @file       Button.h
 * @author     Pere Tuset-Peiro (peretuset@openmote.com)
 * @version    v0.1
 * @date       May, 2014
 * @brief
 * @ingroup
 *
 */

#ifndef BUTTON_H_
#define BUTTON_H_

#include "Gpio.h"

class Button : public Gpio
{
public:
    Button(uint32_t port_, uint8_t pin_, uint32_t edge_);
    void enableInterrupt(void);
private:

};

#endif /* BUTTON_H_ */
