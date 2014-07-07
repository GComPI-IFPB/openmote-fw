/*
 * Copyright 2013 OpenMote Technologies, S.L.
 */

/**
 *
 * @file       Uart.cpp
 * @author     Pere Tuset-Peiro (peretuset@openmote.com)
 * @version    v0.1
 * @date       May, 2014
 * @brief
 * @ingroup
 *
 */

/**********************************include************************************/

#include "Uart.h"

#include "gpio.h"
#include "interrupt.h"
#include "ioc.h"
#include "sys_ctrl.h"
#include "uart.h"

#include "hw_ioc.h"
#include "hw_ints.h"
#include "hw_types.h"
#include "hw_uart.h"

/*********************************variables***********************************/



/**********************************public*************************************/

Uart::Uart(uint32_t peripheral_, uint32_t base_, uint32_t clock_, uint32_t interrupt_, Gpio * rx_, uint32_t rx_ioc_, Gpio * tx_, uint32_t tx_ioc_):
    peripheral(peripheral_), base(base_), clock(clock_), interrupt(interrupt_), rx(rx_), tx(tx_)
{
    // Configure the UART RX pin
    IOCPinConfigPeriphInput(rx->getPort(), rx->getPin(), rx_ioc_);
    GPIOPinTypeUARTInput(rx->getPort(), rx->getPin());

    // Configure the UART TX pin    
    IOCPinConfigPeriphOutput(tx->getPort(), tx->getPin(), tx_ioc_);
    GPIOPinTypeUARTOutput(tx->getPort(), tx->getPin());

    // Enable and reset the UART peripheral
    SysCtrlPeripheralEnable(peripheral);
    SysCtrlPeripheralReset(peripheral);

    // Set IO clock as UART clock source
    UARTClockSourceSet(base, clock);
}

uint32_t Uart::getBase(void)
{
    return base;
}

void Uart::enable(uint32_t baudrate_, uint32_t config_, uint32_t mode_)
{
    // Store the UART baudrate, configuration and mode
    baudrate = baudrate_;
    config   = config_;
    mode     = mode_;
    
    // Configure the UART
    UARTConfigSetExpClk(base, SysCtrlIOClockGet(), baudrate, config);

    // Enable UART hardware
    UARTEnable(base);

    // Disable FIFO as we only use a one-byte buffer
    UARTFIFODisable(base);

    // Raise an interrupt at the end of transmission
    UARTTxIntModeSet(base, mode);
}

void Uart::setRxCallback(callback_t callback_)
{
    rx_callback = callback_;
}

void Uart::setTxCallback(callback_t callback_)
{
    tx_callback = callback_;
}

uint8_t Uart::readByte(void)
{
    int32_t byte;
    byte = UARTCharGet(base);
	return (uint8_t)(byte & 0xFF);
}

uint8_t Uart::readByte(uint8_t * buffer, uint8_t len)
{
    return 0;
}

void Uart::writeByte(uint8_t byte)
{
    UARTCharPut(base, byte);
}

void Uart::writeByte(uint8_t * buffer, uint8_t len)
{
    while(len--) {
        writeByte(*buffer++);
    }
}

void Uart::interruptEnable(void)
{
    // Enable UART RX and TX interrupts
    UARTIntEnable(base, UART_INT_RX | UART_INT_TX);

    // Enable the UART interrupt
    IntEnable(interrupt);
}

void Uart::interruptDisable(void)
{
    // Disable the UART RX and TX interrupts
    UARTIntDisable(base, UART_INT_RX | UART_INT_TX);
    
    // Disable the UART interrupt
    IntDisable(interrupt);
}

/*********************************protected***********************************/

void Uart::interruptHandler(void)
{
    uint32_t status;

    // Read interrupt source
    status = UARTIntStatus(base, true);

    // Clear UART interrupt in the NVIC
    IntPendClear(interrupt);

    // Process TX interrupt
    if(status & UART_INT_TX){
        interruptHandlerTx();
    }

    // Process RX interrupt
    if(status & UART_INT_RX) {
        interruptHandlerRx();
    }
}

/**********************************private************************************/

void Uart::interruptHandlerRx()
{
    if(rx_callback != nullptr)
    {
        rx_callback();
    }
}

void Uart::interruptHandlerTx()
{
    if(tx_callback != nullptr)
    {
        tx_callback();
    }
}
