/*
 * Copyright 2013 OpenMote Technologies, S.L.
 */

/**
 *
 * @file       Button.cpp
 * @author     Pere Tuset-Peiro (peretuset@openmote.com)
 * @version    v0.1
 * @date       May, 2014
 * @brief
 * @ingroup
 *
 */

/**********************************include************************************/

#include "Button.h"

/*********************************variables***********************************/

/**********************************public*************************************/

Button::Button(uint32_t port_, uint8_t pin_, uint32_t edge_):
    Gpio(port_, pin_), edge(edge_)
{
    // Set the pin as input
    GPIOPinTypeGPIOInput(port, pin);
    
    // Set the edge of the interrupt
    GPIOPowIntTypeSet(port, pin, edge);
    
    // Enable the interrupt wakeup capability of the port
    if(port_ == GPIO_A_BASE)
    {
        GPIOIntWakeupEnable(GPIO_IWE_PORT_A);
    }
    else if(port_ == GPIO_B_BASE)
    {
        GPIOIntWakeupEnable(GPIO_IWE_PORT_B);
    }
    else if(port_ == GPIO_C_BASE)
    {
        GPIOIntWakeupEnable(GPIO_IWE_PORT_C);
    }
    else if(port_ == GPIO_D_BASE)
    {
        GPIOIntWakeupEnable(GPIO_IWE_PORT_D);
    }
}

void Button::enableInterrupt(void)
{
    // Clear the power interrupt
    GPIOPowIntClear(port, pin);
    
    // Enable the power intrrupt
    GPIOPowIntEnable(port, pin);
}

void Button::disableInterrupt(void)
{
    // Enable the power intrrupt
    GPIOPowIntDisable(port, pin);
}

/*********************************protected***********************************/

/**********************************private************************************/

