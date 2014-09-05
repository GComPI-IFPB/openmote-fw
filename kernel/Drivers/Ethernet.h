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

class EthernetDevice
{
public:
    EthernetDevice(void);
    virtual void init(uint8_t* mac_address) = 0;
    virtual void reset(void) = 0;
    virtual int32_t sendPacket(uint8_t* data, uint32_t length) = 0;
    virtual int32_t receivePacket(uint8_t* buffer, uint32_t length) = 0;
protected:
    void setMacAddress(uint8_t* mac_address);
protected:
    uint8_t macAddress[6];
    bool isInitialized;
    uint32_t receivedPackets;
    uint32_t receivedPacketsError;
    uint32_t sentPackets;
    uint32_t sentPacketsError;
};

class Ethernet
{
public:
    Ethernet(EthernetDevice& ethernetDevice_);
private:
    EthernetDevice& ethernetDevice;
};

#endif /* ETHERNET_H_ */
