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

Uart::Uart(uint32_t port_, Gpio * rx_, Gpio * tx_):
    port(port_), rx(rx_), tx(tx_)
{
    SysCtrlPeripheralEnable(SYS_CTRL_PERIPH_UART0);

    // Disable
    UARTDisable(UART0_BASE);

    // Disable all UART module interrupts
    UARTIntDisable(UART0_BASE, 0x1FFF);

    // Set IO clock as UART clock source
    UARTClockSourceSet(UART0_BASE, UART_CLOCK_PIOSC);

    // Map UART signals to the correct GPIO pins and configure them as
    // hardware controlled. GPIO-A pin 0 and 1
    IOCPinConfigPeriphOutput(GPIO_A_BASE, GPIO_PIN_1, IOC_MUX_OUT_SEL_UART0_TXD);
    GPIOPinTypeUARTOutput(GPIO_A_BASE, GPIO_PIN_1);
    IOCPinConfigPeriphInput(GPIO_A_BASE, GPIO_PIN_0, IOC_UARTRXD_UART0);
    GPIOPinTypeUARTInput(GPIO_A_BASE, GPIO_PIN_0);
}

uint32_t Uart::getPort(void)
{
    return port;
}

void Uart::init(uint32_t baudrate_, uint32_t config_)
{
    // Configure the UART for 115,200, 8-N-1 operation.
    // This function uses SysCtrlClockGet() to get the system clock
    // frequency.  This could be also be a variable or hard coded value
    // instead of a function call.
    UARTConfigSetExpClk(UART0_BASE, SysCtrlIOClockGet(), 115200, (UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE | UART_CONFIG_PAR_NONE));

    // Enable UART hardware
    UARTEnable(UART0_BASE);

    // Disable FIFO as we only one 1byte buffer
    UARTFIFODisable(UART0_BASE);

    // Raise interrupt at end of tx (not by fifo)
    UARTTxIntModeSet(UART0_BASE, UART_TXINT_MODE_EOT);
}

void Uart::enableInterrupt(void)
{
    // 
    UARTIntEnable(UART0_BASE, UART_INT_RX | UART_INT_TX);

    // Enable the UART0 interrupt
    IntEnable(INT_UART0);
}

void Uart::disableInterrupt(void)
{
    // 
    UARTIntDisable(UART0_BASE, UART_INT_RX | UART_INT_TX);
    
    // Disable the UART0 interrupt
    IntDisable(INT_UART0);
}

uint8_t Uart::readByte(void)
{
    int32_t byte;
    byte = UARTCharGet(UART0_BASE);
	return (uint8_t)(byte & 0xFF);
}

void Uart::writeByte(uint8_t byte)
{
    UARTCharPut(UART0_BASE, byte);
}

void Uart::tx_interrupt()
{
}

void Uart::rx_interrupt()
{
}

void Uart::interruptHandler(void)
{
    uint32_t status;

    // Read interrupt source
    status = UARTIntStatus(port, true);

    // Clear UART interrupt in the NVIC
    IntPendClear(INT_UART0);

    // Process TX interrupt
    if(status & UART_INT_TX){
        tx_interrupt();
    }

    // Process RX interrupt
    if(status & UART_INT_RX) {
        rx_interrupt();
    }
}
