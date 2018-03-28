/**
 * @file       GpioOut.cpp
 * @author     Pere Tuset-Peiro (peretuset@openmote.com)
 * @version    v0.1
 * @date       May, 2015
 * @brief
 *
 * @copyright  Copyright 2015, OpenMote Technologies, S.L.
 *             This file is licensed under the GNU General Public License v2.
 */

/*================================ include ==================================*/

#include "Gpio.h"

#include "cc2538_include.h"
#include "platform_types.h"

/*================================ define ===================================*/

/*================================ typedef ==================================*/

/*=============================== variables =================================*/

/*=============================== prototypes ================================*/

/*================================= public ==================================*/

GpioOut::GpioOut(GpioConfig& config):
    Gpio(config)
{
    // Set the pin as output
    GPIOPinTypeGPIOOutput(config_.port, config_.pin);

    off();
}

void GpioOut::on(void)
{
    if (config_.invert) {
        GPIOPinWrite(config_.port, config_.pin, 0);
    } else {
        GPIOPinWrite(config_.port, config_.pin, config_.pin);
    }
}

void GpioOut::off(void)
{
    if (config_.invert) {
        GPIOPinWrite(config_.port, config_.pin, config_.pin);
    } else {
        GPIOPinWrite(config_.port, config_.pin, 0);
    }
}

void GpioOut::toggle(void)
{
    // Read the old status of the pin
    uint32_t status = GPIOPinRead(config_.port, config_.pin);

    // Change the status of the pin
    status = (~status) & config_.pin;

    // Set the new status of the pin
    GPIOPinWrite(config_.port, config_.pin, status);
}

void GpioOut::high(void)
{
    GPIOPinWrite(config_.port, config_.pin, config_.pin);
}

void GpioOut::low(void)
{
    GPIOPinWrite(config_.port, config_.pin, 0);
}

uint32_t GpioOut::status(void)
{
    // Read the pin status
    uint32_t status = GPIOPinRead(config_.port, config_.pin);

    // Return the pin status
    return (status & config_.pin);
}

/*=============================== protected =================================*/

/*================================ private ==================================*/
