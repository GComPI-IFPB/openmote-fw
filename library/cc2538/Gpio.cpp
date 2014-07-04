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

uint32_t Gpio::getPort(void)
{
    return port;
}

uint8_t Gpio::getPin(void)
{
    return pin;
}

void Gpio::on(void)
{
    // Set the pin high
    GPIOPinWrite(port, pin, pin);
}

void Gpio::off(void)
{
    // Set the pin low
    GPIOPinWrite(port, pin, 0);
}

void Gpio::toggle(void)
{
    // Read the old status of the pin
    uint32_t status = GPIOPinRead(port, pin);
    
    // Change the status of the pin
    status = (~status) & pin;
    
    // Set the new status of the pin
    GPIOPinWrite(port, pin, status);
}

uint32_t Gpio::status(void)
{
    // Read the pin status
    uint32_t status = GPIOPinRead(port, pin);
    
    // Return the pin status
    return (status & pin);
}

void Gpio::setCallback(callback_t callback_)
{
    // Save the pointer to the callback function
    callback = callback_;
    
    // Get a reference to the interruptHandler object
    InterruptHandler & interruptHandler = InterruptHandler::getInstance();
    
    // Register to the interruptHandler by passing a pointer to the object
    interruptHandler.registerInterruptHandler(this);
}

void Gpio::clearCallback(void)
{
    // Clear the pointer to the callback function
    callback = nullptr;
}

void Gpio::enableInterrupt(void)
{
    // Clear the interrupt
    GPIOPinIntClear(port, pin);
    
    // Enable the interrupt
	GPIOPinIntEnable(port, pin);
}

void Gpio::disableInterrupt(void)
{
    // Disable the interrupt
    GPIOPinIntDisable(port, pin);
}

void Gpio::interruptHandler(void)
{
    // Call the interrupt handler if it is NOT null
    if (callback != nullptr) {
        callback();
    }
}
