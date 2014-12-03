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

Gpio::Gpio(uint32_t port, uint8_t pin):
    port_(port), pin_(pin)
{
}

uint32_t Gpio::getPort(void)
{
    return port_;
}

uint8_t Gpio::getPin(void)
{
    return pin_;
}

GpioI2c::GpioI2c(uint32_t port, uint8_t pin):
    Gpio(port, pin)
{
}

GpioSpi::GpioSpi(uint32_t port, uint8_t pin, uint32_t ioc):
    Gpio(port, pin), ioc_(ioc)
{
    GPIOPinTypeGPIOOutput(port_, pin_);
}

uint32_t GpioSpi::getIoc(void)
{
    return ioc_;
}

void GpioSpi::high(void)
{
    GPIOPinWrite(port_, pin_, pin_);
}

void GpioSpi::low(void)
{
    GPIOPinWrite(port_, pin_, 0);
}

GpioUart::GpioUart(uint32_t port, uint8_t pin, uint32_t ioc):
    Gpio(port, pin), ioc_(ioc)
{
}

uint32_t GpioUart::getIoc(void)
{
    return ioc_;
}

/*=============================== protected =================================*/

/*================================ private ==================================*/
