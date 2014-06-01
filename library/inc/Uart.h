/*
 * Copyright 2013 OpenMote Technologies, S.L.
 */

/**
 *
 * @file       Uart.h
 * @author     Pere Tuset-Peiro (peretuset@openmote.com)
 * @version    v0.1
 * @date       May, 2014
 * @brief
 * @ingroup
 *
 */

#ifndef UART_H_
#define UART_H_

class Gpio;

#include "interrupt.h"
#include "ioc.h"
#include "uart.h"
#include "sys_ctrl.h"

#include "hw_ioc.h"
#include "hw_ints.h"
#include "hw_types.h"
#include "hw_uart.h"

class Uart
{
public:
    Uart(uint32_t peripheral_, uint32_t uart_, uint32_t clock_, uint32_t interrupt_);
    void init(uint32_t baudrate_, uint32_t config_, uint32_t mode);
    uint32_t getPort(void);
    void setRxGpio(Gpio & rx_, uint32_t ioc_);
    void setTxGpio(Gpio & tx_, uint32_t ioc_);
    uint8_t readByte(void);
    void writeByte(uint8_t byte);
    void interruptEnable(void);
    void interruptDisable(void);
    void interruptHandler(void);
    void interruptHandlerRx();
    void interruptHandlerTx();
private:
    uint32_t peripheral;
    uint32_t uart;
    uint32_t clock;
    uint32_t interrupt;
    uint32_t baudrate;
    uint32_t config;
    uint32_t mode;
    Gpio * rx;
    Gpio * tx;
};

#endif /* UART_H_ */
