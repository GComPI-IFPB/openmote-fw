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

#include "GpioIn.h"
#include "InterruptHandler.h"

#include "cc2538_include.h"

/*================================ define ===================================*/

/*================================ typedef ==================================*/

/*=============================== variables =================================*/

/*=============================== prototypes ================================*/

/*================================= public ==================================*/

GpioIn::GpioIn(uint32_t port_, uint8_t pin_, uint32_t edge_):
    Gpio(port_, pin_), edge(edge_)
{
    // Set the pin as input
    GPIOPinTypeGPIOInput(port, pin);

    // Set the edge of the interrupt
    GPIOIntTypeSet(port, pin, edge);
}

bool GpioIn::read(void)
{
    uint32_t state;
    state = GPIOPinRead(port, pin);
    return (bool) state;
}

void GpioIn::setCallback(Callback* callback_)
{
    // Save the pointer to the callback function
    callback = callback_;

    // Get a reference to the interruptHandler object
    InterruptHandler& interruptHandler = InterruptHandler::getInstance();

    // Register to the interruptHandler by passing a pointer to the object
    interruptHandler.setInterruptHandler(this);
}

void GpioIn::clearCallback(void)
{
    // Clear the pointer to the callback function
    callback = nullptr;
}

void GpioIn::enableInterrupt(void)
{
    // Clear the interrupt
    GPIOPinIntClear(port, pin);

    // Enable the interrupt
    GPIOPinIntEnable(port, pin);
}

void GpioIn::disableInterrupt(void)
{
    // Disable the interrupt
    GPIOPinIntDisable(port, pin);
}

void GpioIn::interruptHandler(void)
{
    // Call the interrupt handler if it is NOT null
    if (callback != nullptr) {
        callback->execute();
    }
}

/*=============================== protected =================================*/

/*================================ private ==================================*/
