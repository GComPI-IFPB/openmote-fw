/*
 * Copyright 2013 OpenMote Technologies, S.L.
 */

/**
 *
 * @file       GpioOut.cpp
 * @author     Pere Tuset-Peiro (peretuset@openmote.com)
 * @version    v0.1
 * @date       May, 2014
 * @brief
 * @ingroup
 *
 */

/**********************************include************************************/

#include "GpioOut.h"

#include "gpio.h"

#include "hw_memmap.h"
#include "hw_types.h"

/*********************************variables***********************************/



/**********************************public*************************************/

GpioOut::GpioOut(uint32_t port_, uint8_t pin_):
    Gpio(port_, pin_)
{
    // Set the pin as output
    GPIOPinTypeGPIOOutput(port, pin);
    
    // Set the pin low
    GPIOPinWrite(port, pin, 0);
}

void GpioOut::on(void)
{
    // Set the pin high
    GPIOPinWrite(port, pin, pin);
}

void GpioOut::off(void)
{
    // Set the pin low
    GPIOPinWrite(port, pin, 0);
}

void GpioOut::toggle(void)
{
    // Read the old status of the pin
    uint32_t status = GPIOPinRead(port, pin);
    
    // Change the status of the pin
    status = (~status) & pin;
    
    // Set the new status of the pin
    GPIOPinWrite(port, pin, status);
}

uint32_t GpioOut::status(void)
{
    // Read the pin status
    uint32_t status = GPIOPinRead(port, pin);
    
    // Return the pin status
    return (status & pin);
}

/*********************************protected***********************************/

/**********************************private************************************/

