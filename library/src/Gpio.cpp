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
#include "InterruptHandler.h"

Gpio::Gpio(uint32_t port_, uint8_t pin_):
    port(port_), pin(pin_)
{
}

Gpio::Gpio(uint32_t port_, uint8_t pin_, uint32_t edge_):
    port(port_), pin(pin_), edge(edge_)
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

void Gpio::setCallback(callback_t callback_)
{
    callback = callback_;
    
    InterruptHandler & interruptHandler = InterruptHandler::getInstance();
    interruptHandler.registerGpioInterruptHandler(this);
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

void Gpio::interrupt(void)
{
    if (callback != nullptr) {
        callback();
    }
}
