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
    Uart(uint32_t port, Gpio * rx_, Gpio * tx_);
    void init(uint32_t baudrate, uint32_t config);
    uint32_t getPort(void);
    uint8_t readByte(void);
    void writeByte(uint8_t byte);
    void enableInterrupt(void);
    void disableInterrupt(void);
    void tx_interrupt();
    void rx_interrupt();
    void interruptHandler(void);
private:
    uint32_t port;
    uint32_t baudrate;
    uint32_t config;
    Gpio * rx;
    Gpio * tx;
};

#endif /* UART_H_ */
