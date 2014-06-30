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

#include "Button.h"

Button::Button(uint32_t port_, uint8_t pin_, uint32_t edge_):
    Gpio(port_, pin_, edge_)
{
    GPIOPinTypeGPIOInput(port, pin);
    GPIOPowIntTypeSet(port, pin, edge);
    
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
    GPIOPowIntClear(port, pin);
    GPIOPowIntEnable(port, pin);
}
