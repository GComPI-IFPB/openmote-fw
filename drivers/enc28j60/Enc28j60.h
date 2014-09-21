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

#include "SpiDriver.h"
#include "GpioIn.h"

#include "Ethernet.h"

#include "Callback.h"

class Enc28j60;

typedef GenericCallback<Enc28j60> Enc28j60Callback;

class Enc28j60 : public EthernetDevice
{
public:
    Enc28j60(SpiDriver& spi_, GpioIn& gpio_);
    void init(uint8_t* mac_address);
    void reset(void);
    OperationResult transmitFrame(uint8_t* data, uint16_t length);
    OperationResult receiveFrame(uint8_t* buffer, uint16_t* length);
protected:
    void interruptHandler(void);
private:
    uint8_t readOp(uint8_t op, uint8_t address);
    void writeOp(uint8_t op, uint8_t address, uint8_t data);
    void setBank(uint8_t address);
    uint8_t readRegByte (uint8_t address);
    void writeRegByte(uint8_t address, uint8_t data);
    uint16_t readReg(uint8_t address);
    void writeReg(uint8_t address, uint16_t data);
    uint16_t readPhyByte(uint8_t address);
    void writePhy(uint8_t address, uint16_t data);
    void writeBuf(const uint8_t* data, uint16_t len);
    void readBuf(uint8_t* data, uint16_t len);
    bool isLinkUp(void);
private:
    SpiDriver& spi;
    GpioIn& gpio;

    Enc28j60Callback callback;

    uint8_t  currentBank;
    uint32_t nextPacketPtr;
};

#endif /* ENC268J60_H_ */
