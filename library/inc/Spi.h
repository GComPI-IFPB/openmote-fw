/*
 * Copyright 2013 OpenMote Technologies, S.L.
 */

/**
 *
 * @file       Spi.h
 * @author     Pere Tuset-Peiro (peretuset@openmote.com)
 * @version    v0.1
 * @date       May, 2014
 * @brief
 * @ingroup
 *
 */

#ifndef SPI_H_
#define SPI_H_

#include <stdint.h>

#include "Gpio.h"
#include "Callback.h"

class Gpio;

class Spi
{

friend class InterruptHandler;

public:
    Spi(uint32_t peripheral_, uint32_t base_, uint32_t clock_, \
        GpioSpi* miso_, GpioSpi* mosi_, GpioSpi* clk_, GpioSpi* ncs_);
    uint32_t getBase(void);
    void enable(uint32_t mode_, uint32_t protocol_, uint32_t datawidth_, uint32_t baudrate_);
    void sleep(void);
    void wakeup(void);
    void setRxCallback(Callback* callback_);
    void setTxCallback(Callback* callback_);
    void interruptEnable(void);
    void interruptDisable(void);
    uint8_t readByte(void);
    uint32_t readByte(uint8_t * buffer, uint32_t length);
    void writeByte(uint8_t byte);
    uint32_t writeByte(uint8_t * buffer, uint32_t length);
protected:
    void interruptHandler(void);
private:
    void interruptHandlerRx();
    void interruptHandlerTx();
private:
    uint32_t peripheral;
    uint32_t base;
    uint32_t clock;
    uint32_t interrupt;
    uint32_t mode;
    uint32_t protocol;
    uint32_t baudrate;
    uint32_t datawidth;

    GpioSpi* miso;
    GpioSpi* mosi;
    GpioSpi* clk;
    GpioSpi* ncs;

    Callback* rx_callback;
    Callback* tx_callback;
};

#endif /* SPI_H_ */
