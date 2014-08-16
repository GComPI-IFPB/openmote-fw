/*
 * Copyright 2013 OpenMote Technologies, S.L.
 */

/**
 *
 * @file       GpioIn.cpp
 * @author     Pere Tuset-Peiro (peretuset@openmote.com)
 * @version    v0.1
 * @date       May, 2014
 * @brief
 * @ingroup
 *
 */

/*================================ include ==================================*/

#include "GpioInPow.h"

#include "gpio.h"

#include "hw_memmap.h"
#include "hw_types.h"

/*================================ define ===================================*/

/*================================ typedef ==================================*/

/*=============================== variables =================================*/

/*=============================== prototypes ================================*/

/*================================= public ==================================*/


GpioInPow::GpioInPow(uint32_t port_, uint8_t pin_, uint32_t edge_):
    GpioIn(port_, pin_, edge_)
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

void GpioInPow::enableInterrupt(void)
{
    // Clear the power interrupt
    GPIOPowIntClear(port, pin);

    // Enable the power intrrupt
    GPIOPowIntEnable(port, pin);
}

void GpioInPow::disableInterrupt(void)
{
    // Enable the power intrrupt
    GPIOPowIntDisable(port, pin);
}

/*=============================== protected =================================*/

/*================================ private ==================================*/
