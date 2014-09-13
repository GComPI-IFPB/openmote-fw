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
    ethernetDevice(ethernetDevice_), \
    receivedPackets(0), receivedPacketsError(0), \
    sentPackets(0), sentPacketsError(0)
{
}

/*=============================== protected =================================*/

/*================================ private ==================================*/
