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

#include "Uart.h"
#include "Gpio.h"

Uart::Uart(uint32_t peripheral_, uint32_t uart_, uint32_t clock_, uint32_t interrupt_):
    peripheral(peripheral_), uart(uart_), clock(clock_), interrupt(interrupt_)
{
    // Enable the UART peripheral
    SysCtrlPeripheralEnable(peripheral);
    SysCtrlPeripheralReset(peripheral);

    // Set IO clock as UART clock source
    UARTClockSourceSet(uart, clock);
}

void Uart::setRxGpio(Gpio & rx_, uint32_t ioc_)
{
    rx = &rx_;
    IOCPinConfigPeriphInput(rx->getPort(), rx->getPin(), ioc_);
    GPIOPinTypeUARTInput(rx->getPort(), rx->getPin());
}

void Uart::setTxGpio(Gpio & tx_, uint32_t ioc_)
{
    tx = &tx_;
    IOCPinConfigPeriphOutput(tx->getPort(), tx->getPin(), ioc_);
    GPIOPinTypeUARTOutput(tx->getPort(), tx->getPin());
}

uint32_t Uart::getPort(void)
{
    return uart;
}

void Uart::init(uint32_t baudrate_, uint32_t config_, uint32_t mode_)
{
    // Store the UART baudrate, configuration and mode
    baudrate = baudrate_;
    config   = config_;
    mode     = mode_;
    
    // Configure the UART
    UARTConfigSetExpClk(uart, SysCtrlIOClockGet(), baudrate, config);

    // Enable UART hardware
    UARTEnable(uart);

    // Disable FIFO as we only use a one-byte buffer
    UARTFIFODisable(uart);

    // Raise an interrupt at the end of transmission
    UARTTxIntModeSet(uart, mode);
}

uint8_t Uart::readByte(void)
{
    int32_t byte;
    byte = UARTCharGet(uart);
	return (uint8_t)(byte & 0xFF);
}

void Uart::writeByte(uint8_t byte)
{
    UARTCharPut(uart, byte);
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
    UARTIntEnable(uart, UART_INT_RX | UART_INT_TX);

    // Enable the UART interrupt
    IntEnable(interrupt);
}

void Uart::interruptDisable(void)
{
    // Disable the UART RX and TX interrupts
    UARTIntDisable(uart, UART_INT_RX | UART_INT_TX);
    
    // Disable the UART interrupt
    IntDisable(interrupt);
}

void Uart::interruptHandler(void)
{
    uint32_t status;

    // Read interrupt source
    status = UARTIntStatus(uart, true);

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

void Uart::interruptHandlerRx()
{
}

void Uart::interruptHandlerTx()
{
}
