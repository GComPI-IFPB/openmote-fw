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

#include "SpiDriver.h"
#include "GpioIn.h"

class Enc28j60
{
public:
    Enc28j60(SpiDriver& spi_, GpioIn& gpio_);
    void init(uint8_t* address);
    void reset(void);
    int32_t send(uint8_t* data, uint32_t length);
    int32_t read(uint8_t* buffer, uint32_t length);
private:
    void setRegBank(uint8_t bank);
    uint8_t readRegister(uint8_t address);
    void writeRegister(uint8_t address, uint8_t data);
    uint8_t readDataByte(void);
    void writeDataByte(uint8_t byte);
    int32_t readData(uint8_t* buffer, uint32_t length);
    void writeData(uint8_t* data, uint32_t length);
    void softReset(void);
private:
    SpiDriver& spi;
    GpioIn& gpio;

    uint8_t mac_address[6];
};
