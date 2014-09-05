/*
 * Copyright 2013 OpenMote Technologies, S.L.
 */

/**
 *
 * @file       Ethernet.cpp
 * @author     Pere Tuset-Peiro (peretuset@openmote.com)
 * @version    v0.1
 * @date       May, 2014
 * @brief
 * @ingroup
 *
 */

/*================================ include ==================================*/

#include "Ethernet.h"

/*================================ define ===================================*/

/*================================ typedef ==================================*/

/*=============================== variables =================================*/

/*=============================== prototypes ================================*/

/*================================= public ==================================*/

Ethernet::Ethernet(EthernetDevice& ethernetDevice_):
    ethernetDevice(ethernetDevice_)
{
}

EthernetDevice::EthernetDevice(void):
    isInitialized(false), \
    receivedPackets(0), receivedPacketsError(0), \
    sentPackets(0), sentPacketsError(0)
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
