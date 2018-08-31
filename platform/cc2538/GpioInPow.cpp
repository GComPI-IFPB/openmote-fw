/**
 * @file       GpioInPow.cpp
 * @author     Pere Tuset-Peiro (peretuset@openmote.com)
 * @version    v0.1
 * @date       May, 2015
 * @brief
 *
 * @copyright  Copyright 2015, OpenMote Technologies, S.L.
 *             This file is licensed under the GNU General Public License v2.
 */

/*================================ include ==================================*/

#include <Gpio.hpp>
#include "cc2538_include.h"
#include "platform_types.h"

/*================================ define ===================================*/

/*================================ typedef ==================================*/

/*=============================== variables =================================*/

/*=============================== prototypes ================================*/

/*================================= public ==================================*/

GpioInPow::GpioInPow(GpioConfig& config):
    GpioIn(config)
{
    // Set the pin as input
    GPIOPinTypeGPIOInput(config_.port, config_.pin);

    // Set the edge of the interrupt
    GPIOPowIntTypeSet(config_.port, config_.pin, config_.edge);

    // Enable the interrupt wakeup capability of the port
    if(config_.port == GPIO_A_BASE)
    {
        GPIOIntWakeupEnable(GPIO_IWE_PORT_A);
    }
    else if(config_.port == GPIO_B_BASE)
    {
        GPIOIntWakeupEnable(GPIO_IWE_PORT_B);
    }
    else if(config_.port == GPIO_C_BASE)
    {
        GPIOIntWakeupEnable(GPIO_IWE_PORT_C);
    }
    else if(config_.port == GPIO_D_BASE)
    {
        GPIOIntWakeupEnable(GPIO_IWE_PORT_D);
    }
}

void GpioInPow::enableInterrupts(void)
{
    // Clear the power interrupt
    GPIOPowIntClear(config_.port, config_.pin);

    // Enable the power intrrupt
    GPIOPowIntEnable(config_.port, config_.pin);
}

void GpioInPow::disableInterrupts(void)
{
    // Enable the power intrrupt
    GPIOPowIntDisable(config_.port, config_.pin);
}

/*=============================== protected =================================*/

/*================================ private ==================================*/
