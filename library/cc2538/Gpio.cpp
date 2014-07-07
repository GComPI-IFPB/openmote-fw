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

/**********************************include************************************/

#include "Gpio.h"

/*********************************variables***********************************/

/**********************************public*************************************/

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

/*********************************protected***********************************/

/**********************************private************************************/

