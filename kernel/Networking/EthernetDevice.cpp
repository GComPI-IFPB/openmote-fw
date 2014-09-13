/*
 * Copyright 2013 OpenMote Technologies, S.L.
 */

/**
 *
 * @file       EthernetDevice.cpp
 * @author     Pere Tuset-Peiro (peretuset@openmote.com)
 * @version    v0.1
 * @date       May, 2014
 * @brief
 * @ingroup
 *
 */

/*================================ include ==================================*/

#include "EthernetDevice.h"

/*================================ define ===================================*/

/*================================ typedef ==================================*/

/*=============================== variables =================================*/

/*=============================== prototypes ================================*/

/*================================= public ==================================*/

EthernetDevice::EthernetDevice(void):
    isInitialized(false)
{
}

/*=============================== protected =================================*/

void EthernetDevice::setMacAddress(uint8_t* mac_address)
{
    for (uint8_t i = 0; i < 6; i++)
    {
        macAddress[i] = mac_address[i];
    }
}

/*================================ private ==================================*/
