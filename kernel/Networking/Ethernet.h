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
#include "EthernetFrame.h"

#include "Callback.h"

class Ethernet;

typedef GenericCallback<Ethernet> EthernetCallback;

class Ethernet
{
public:
    Ethernet(EthernetDevice& ethernetDevice_);
    void init(uint8_t* mac_address);
    void transmitFrame(uint8_t* frame, uint32_t length);
    void receiveFrame(uint8_t* buffer, uint32_t length);
private:
    void interruptHandler(void);
private:
    EthernetDevice& ethernetDevice;

    EthernetCallback interrupt;

    uint8_t rx_buffer[1518];
    uint8_t* rx_buffer_ptr;
    uint32_t rx_buffer_len;

    uint32_t receivedFrames;
    uint32_t receivedFramesError;
    uint32_t sentFrames;
    uint32_t sentFramesError;
};

#endif /* ETHERNET_H_ */
