/*
 * Copyright 2013 OpenMote Technologies, S.L.
 */

/**
 *
 * @file       Fsa.cpp
 * @author     Pere Tuset-Peiro (peretuset@openmote.com)
 * @version    v0.1
 * @date       May, 2014
 * @brief
 * @ingroup
 *
 */

/*================================ include ==================================*/

#include "Lpdq.h"

/*================================ define ===================================*/

/*================================ typedef ==================================*/

/*=============================== prototypes ================================*/

/*=============================== variables =================================*/

static const uint8_t DEFAULT_CHANNEL = 11;
static const uint8_t DEFAULT_POWER   = 0xFF;

/*================================= public ==================================*/

Lpdq::Lpdq(Radio& radio_):
    radio(radio_), \
    rxInitCallback(this, &Lpdq::rxInit), rxDoneCallback(this, &Lpdq::rxDone), \
    txInitCallback(this, &Lpdq::txInit), txDoneCallback(this, &Lpdq::txDone)
{

}

void Lpdq::init(void)
{
    radio.enable();
    
    radio.setRxCallbacks(&rxInitCallback, &rxDoneCallback);
    radio.setRxCallbacks(&txInitCallback, &txDoneCallback);
    radio.enableInterrupts();
    
    radio.setChannel(DEFAULT_CHANNEL);
    radio.setPower(DEFAULT_POWER);
}

/*=============================== protected =================================*/

void Lpdq::rxInit(void)
{
}

void Lpdq::rxDone(void)
{
}

void Lpdq::txInit(void)
{
}

void Lpdq::txDone(void)
{
}

/*================================ private ==================================*/

