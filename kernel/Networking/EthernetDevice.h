/*
 * Copyright 2013 OpenMote Technologies, S.L.
 */

/**
 *
 * @file       EthernetDevice.h
 * @author     Pere Tuset-Peiro (peretuset@openmote.com)
 * @version    v0.1
 * @date       May, 2014
 * @brief
 * @ingroup
 *
 */

#ifndef ETHERNET_DEVICE_H_
#define ETHERNET_DEVICE_H_

#include <stdint.h>

enum OperationResult : int8_t
{
    ResultError   = -1,
    ResultSuccess =  0
};

class EthernetDevice
{
public:
    EthernetDevice(void);
    virtual void init(uint8_t* mac_address) = 0;
    virtual void reset(void) = 0;
    virtual OperationResult transmitFrame(uint8_t* data, uint32_t length) = 0;
    virtual OperationResult receiveFrame(uint8_t* buffer, uint32_t* length) = 0;
protected:
    void setMacAddress(uint8_t* mac_address);
protected:
    bool isInitialized;
    uint8_t macAddress[6];
};

#endif /* ETHERNET_DEVICE_H_ */
