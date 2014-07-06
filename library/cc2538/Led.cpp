/*
 * Copyright 2013 OpenMote Technologies, S.L.
 */

/**
 *
 * @file       Led.cpp
 * @author     Pere Tuset-Peiro (peretuset@openmote.com)
 * @version    v0.1
 * @date       May, 2014
 * @brief
 * @ingroup
 *
 */

/**********************************include************************************/

#include "Led.h"

/*********************************variables***********************************/



/**********************************public*************************************/

Led::Led(uint32_t port_, uint8_t pin_):
    Gpio(port_, pin_)
{
    // Set the pin as output
    GPIOPinTypeGPIOOutput(port, pin);
    
    // Set the pin low
    GPIOPinWrite(port, pin, 0);
}

/*********************************protected***********************************/

/**********************************private************************************/

