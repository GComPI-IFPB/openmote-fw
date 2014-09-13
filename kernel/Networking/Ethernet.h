/*
 * Copyright 2013 OpenMote Technologies, S.L.
 */

/**
 *
 * @file       Ethernet.h
 * @author     Pere Tuset-Peiro (peretuset@openmote.com)
 * @version    v0.1
 * @date       May, 2014
 * @brief
 * @ingroup
 *
 */

#ifndef ETHERNET_H_
#define ETHERNET_H_

#include <stdint.h>

#include "EthernetDevice.h"

class Ethernet
{
public:
    Ethernet(EthernetDevice& ethernetDevice_);
private:
    EthernetDevice& ethernetDevice;

    uint32_t receivedPackets;
    uint32_t receivedPacketsError;
    uint32_t sentPackets;
    uint32_t sentPacketsError;
};

#endif /* ETHERNET_H_ */
