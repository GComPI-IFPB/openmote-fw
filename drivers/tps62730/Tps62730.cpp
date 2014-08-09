/*
 * Copyright 2013 OpenMote Technologies, S.L.
 */

/**
 *
 * @file       tps62730.cpp
 * @author     Pere Tuset-Peiro (peretuset@openmote.com)
 * @version    v0.1
 * @date       May, 2014
 * @brief
 * @ingroup
 *
 */

/**********************************include************************************/

#include "Tps62730.h"

/**********************************defines************************************/

/*********************************variables***********************************/

/**********************************public*************************************/

Tps62730::Tps62730(GpioOut* bypass_, GpioIn* status_):
    bypass(bypass_), status(status_)
{
}

void Tps62730::setOn(void)
{
    bypass->on();
}

void Tps62730::setBypass(void)
{
    bypass->off();
}

bool Tps62730::getStatus(void)
{
    bool state;
    state = status->read();
    return state;
}

/*********************************protected***********************************/

/**********************************private************************************/

