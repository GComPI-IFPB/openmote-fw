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
#include "InterruptHandler.h"

#include "gpio.h"
#include "interrupt.h"
#include "ioc.h"
#include "sys_ctrl.h"
#include "uart.h"

#include "hw_ioc.h"
#include "hw_ints.h"
#include "hw_types.h"
#include "hw_uart.h"

/**********************************defines************************************/



/*********************************variables***********************************/



/**********************************public*************************************/

Uart::Uart(uint32_t peripheral_, uint32_t base_, uint32_t clock_, uint32_t interrupt_, GpioUart * rx_, GpioUart * tx_):
    peripheral(peripheral_), base(base_), clock(clock_), interrupt(interrupt_), rx(rx_), tx(tx_)
{
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
    
    // Enable peripheral except in sleep and deep sleep modes
    SysCtrlPeripheralEnable(peripheral);
    SysCtrlPeripheralSleepDisable(peripheral);
    SysCtrlPeripheralDeepSleepDisable(peripheral);

    // Reset peripheral previous to configuring it
    UARTDisable(peripheral);

    // Set IO clock as UART clock source
    UARTClockSourceSet(base, clock);

    // Configure the UART RX and TX pins
    IOCPinConfigPeriphInput(rx->getPort(), rx->getPin(), rx->getIoc());
    IOCPinConfigPeriphOutput(tx->getPort(), tx->getPin(), tx->getIoc());

    // Configure the UART GPIOs
    GPIOPinTypeUARTInput(rx->getPort(), rx->getPin());
    GPIOPinTypeUARTOutput(tx->getPort(), tx->getPin());

    // Configure the UART
    UARTConfigSetExpClk(base, SysCtrlIOClockGet(), baudrate, config);

    // Disable FIFO as we only use a one-byte buffer
    UARTFIFODisable(base);

    // Raise an interrupt at the end of transmission
    UARTTxIntModeSet(base, mode);

    // Enable UART hardware
    UARTEnable(base);
}

void Uart::sleep(void)
{
    GPIOPinTypeGPIOOutput(rx->getPort(), rx->getPin());
    GPIOPinTypeGPIOOutput(tx->getPort(), tx->getPin());

    GPIOPinWrite(rx->getPort(), rx->getPin(), 0);
    GPIOPinWrite(tx->getPort(), tx->getPin(), 0);
}

void Uart::wakeup(void)
{
    enable(baudrate, config, mode);
}

void Uart::setRxCallback(callback_t callback_)
{
    rx_callback = callback_;
}

void Uart::setTxCallback(callback_t callback_)
{
    tx_callback = callback_;
}

void Uart::interruptEnable(void)
{
    // Register the interrupt handler
    InterruptHandler::getInstance().registerInterruptHandler(this);

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

uint8_t Uart::readByte(void)
{
    int32_t byte;
    byte = UARTCharGet(base);
	return (uint8_t)(byte & 0xFF);
}

uint8_t Uart::readByte(uint8_t * buffer, uint8_t length)
{
    uint32_t data;
    for (uint32_t i = 0; i < length; i++)
    {
        data = UARTCharGet(base);
        *buffer++ = (uint8_t) data;
    }

    // Wait until it is complete
    while(UARTBusy(base))
        ;

    return 0;
}

void Uart::writeByte(uint8_t byte)
{
    UARTCharPut(base, byte);
}

void Uart::writeByte(uint8_t * buffer, uint8_t length)
{
    for (uint32_t i = 0; i < length; i++)
    {
        UARTCharPut(base, *buffer++);
    }

    // Wait until it is complete
    while(UARTBusy(base))
        ;
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
