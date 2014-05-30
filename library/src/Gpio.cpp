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

#include "Gpio.h"

Gpio::Gpio(uint32_t port_, uint8_t pin_):
    port(port_), pin(pin_)
{
}

Gpio::Gpio(uint32_t port_, uint8_t pin_, uint32_t edge_):
    port(port_), pin(pin_), edge(edge_)
{
}

void Gpio::registerCallback(callback_t callback_)
{
    InterruptHandler & interruptHandler = InterruptHandler::getInstance();
    interruptHandler.registerInterruptHandler(port, pin, callback_);
}

void Gpio::enableInterrupt(void)
{
    GPIOPinIntClear(port, pin);
	GPIOPinIntEnable(port, pin);
}

void Gpio::disableInterrupt(void)
{
    GPIOPinIntDisable(port, pin);
}
