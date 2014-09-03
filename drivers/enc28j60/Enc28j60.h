/*
 * Copyright 2013 OpenMote Technologies, S.L.
 */

/**
 *
 * @file       Enc28j60.h
 * @author     Pere Tuset-Peiro (peretuset@openmote.com)
 * @version    v0.1
 * @date       May, 2014
 * @brief
 * @ingroup
 *
 */

#ifndef ENC268J60_H_
#define ENC268J60_H_

#include <stdint.h>
#include <string.h>

#include "SpiDriver.h"
#include "GpioIn.h"

class Enc28j60
{
public:
    Enc28j60(SpiDriver& spi_, GpioIn& gpio_);
    void init(uint8_t* mac_address);
    void reset(void);
    int32_t sendPacket(uint8_t* data, uint32_t length);
    int32_t receivePacket(uint8_t* buffer, uint32_t length);
protected:
    void setMacAddress(uint8_t* mac_address);
private:
    void softReset(void);
    void setRegBank(uint8_t bank);
    uint8_t readRegister(uint8_t address);
    void writeRegister(uint8_t address, uint8_t data);
    uint8_t readDataByte(void);
    void writeDataByte(uint8_t byte);
    uint32_t readData(uint8_t* buffer, uint32_t length);
    void writeData(uint8_t* data, uint32_t length);
private:
    SpiDriver& spi;
    GpioIn& gpio;

    uint8_t macAddress[6];

    bool isInitialized;
    uint32_t receivedPackets;
    uint32_t receivedPacketsError;
    uint32_t sentPackets;
    uint32_t sentPacketsError;
};

#endif /* ENC268J60_H_ */
