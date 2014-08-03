/*
 * Copyright 2013 OpenMote Technologies, S.L.
 */

/**
 *
 * @file       GpioPwm.cpp
 * @author     Pere Tuset-Peiro (peretuset@openmote.com)
 * @version    v0.1
 * @date       May, 2014
 * @brief
 * @ingroup
 *
 */

/**********************************include************************************/

#include "GpioPwm.h"

/**********************************defines************************************/



/*********************************variables***********************************/



/**********************************public*************************************/

GpioPwm::GpioPwm(uint32_t port_, uint8_t pin_):
    Gpio(port_, pin_)
{
}

/*********************************protected***********************************/



/**********************************private************************************/

