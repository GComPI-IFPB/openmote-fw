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
	GPIOIntTypeSet(port, pin, edge);
}
