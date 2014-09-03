/*
 * Copyright 2013 OpenMote Technologies, S.L.
 */

/**
 *
 * @file       Gpio.cpp
 * @author     Pere Tuset-Peiro (peretuset@openmote.com)
 * @version    v0.1
 * @date       May, 2014
 * @brief
 * @ingroup
 *
 */

/*================================ include ==================================*/

#include "Gpio.h"

#include "cc2538_include.h"

/*================================ define ===================================*/

/*================================ typedef ==================================*/

/*=============================== variables =================================*/

/*=============================== prototypes ================================*/

/*================================= public ==================================*/

Gpio::Gpio(uint32_t port_, uint8_t pin_):
    port(port_), pin(pin_)
{
}

uint32_t Gpio::getPort(void)
{
    return port;
}

uint8_t Gpio::getPin(void)
{
    return pin;
}

GpioI2c::GpioI2c(uint32_t port_, uint8_t pin_):
    Gpio(port_, pin_)
{
}

GpioSpi::GpioSpi(uint32_t port_, uint8_t pin_, uint32_t ioc_):
    Gpio(port_, pin_), ioc(ioc_)
{
    GPIOPinTypeGPIOOutput(port, pin);
}

uint32_t GpioSpi::getIoc(void)
{
    return ioc;
}

void GpioSpi::high(void)
{
    GPIOPinWrite(port, pin, pin);
}

void GpioSpi::low(void)
{
    GPIOPinWrite(port, pin, 0);
}

GpioUart::GpioUart(uint32_t port_, uint8_t pin_, uint32_t ioc_):
    Gpio(port_, pin_), ioc(ioc_)
{
}

uint32_t GpioUart::getIoc(void)
{
    return ioc;
}

/*=============================== protected =================================*/

/*================================ private ==================================*/
