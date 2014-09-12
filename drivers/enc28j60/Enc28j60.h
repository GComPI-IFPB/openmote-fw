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

class Enc28j60;

class Enc28j60Interrupt : public Callback
{
public:
    Enc28j60Interrupt(Enc28j60* object_ = nullptr, \
                   void(Enc28j60:: *method_)(void) = nullptr):
                   object(object_), method(method_){}
    void execute(void) {(object->*method)();}
private:
    Enc28j60* object;
    void(Enc28j60:: *method)(void);
};

class Enc28j60 : public EthernetDevice
{
public:
    Enc28j60(SpiDriver& spi_, GpioIn& gpio_);
    void init(uint8_t* mac_address);
    void reset(void);
    Result transmitFrame(uint8_t* data, uint32_t length);
    Result receiveFrame(uint8_t* buffer, uint32_t* length);
protected:
    void interruptHandler(void);
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

    Enc28j60Interrupt callback;
};

#endif /* ENC268J60_H_ */
