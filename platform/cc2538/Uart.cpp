/**
 * @file       Uart.cpp
 * @author     Pere Tuset-Peiro (peretuset@openmote.com)
 * @version    v0.1
 * @date       May, 2015
 * @brief
 *
 * @copyright  Copyright 2015, OpenMote Technologies, S.L.
 *             This file is licensed under the GNU General Public License v2.
 */

/*================================ include ==================================*/

#include "Gpio.h"
#include "Uart.h"
#include "InterruptHandler.h"

#include "cc2538_include.h"
#include "platform_types.h"

/*================================ define ===================================*/

/*================================ typedef ==================================*/

/*=============================== variables =================================*/

/*=============================== prototypes ================================*/

/*================================= public ==================================*/

Uart::Uart(Gpio& rx, Gpio& tx, UartConfig& config):
    rx_(rx), tx_(tx), config_(config)
{
}

void Uart::enable(uint32_t baudrate)
{
    GpioConfig& rx = rx_.getGpioConfig();
    GpioConfig& tx = tx_.getGpioConfig();

    // Store baudrate in configuration
    if (baudrate != 0) {
        config_.baudrate = baudrate;
    }

    // Enable peripheral except in deep sleep modes (e.g. LPM1, LPM2, LPM3)
    SysCtrlPeripheralEnable(config_.peripheral);
    SysCtrlPeripheralSleepEnable(config_.peripheral);
    SysCtrlPeripheralDeepSleepDisable(config_.peripheral);

    // Disable peripheral previous to configuring it
    UARTDisable(config_.peripheral);

    // Set IO clock as UART clock source
    UARTClockSourceSet(config_.base, config_.clock);

    // Configure the UART RX and TX pins
    IOCPinConfigPeriphInput(rx.port, rx.pin, rx.ioc);
    IOCPinConfigPeriphOutput(tx.port, tx.pin, tx.ioc);

    // Configure the UART GPIOs
    GPIOPinTypeUARTInput(rx.port, rx.pin);
    GPIOPinTypeUARTOutput(tx.port, tx.pin);

    // Configure the UART
    UARTConfigSetExpClk(config_.base, SysCtrlIOClockGet(), config_.baudrate, config_.mode);

    // Disable FIFO as we only use a one-byte buffer
    UARTFIFODisable(config_.base);

    // Raise an interrupt at the end of transmission
    UARTTxIntModeSet(config_.base, UART_TXINT_MODE_EOT);

    // Enable UART hardware
    UARTEnable(config_.base);
}

void Uart::sleep(void)
{
    GpioConfig& rx = rx_.getGpioConfig();
    GpioConfig& tx = tx_.getGpioConfig();

    // Wait until UART is not busy
    while(UARTBusy(config_.base))
        ;

    // Disable UART hardware
    UARTDisable(config_.base);

    // Configure the pins as outputs
    GPIOPinTypeGPIOOutput(rx.port, rx.pin);
    GPIOPinTypeGPIOOutput(tx.port, tx.pin);

    // Pull the pins to ground
    GPIOPinWrite(rx.port, rx.pin, 0);
    GPIOPinWrite(tx.port, tx.pin, 0);
}

void Uart::wakeup(void)
{
    // Re-enable the UART interface
    enable();
}

void Uart::setRxCallback(Callback* callback)
{
    rx_callback_ = callback;
}

void Uart::setTxCallback(Callback* callback)
{
    tx_callback_ = callback;
}

void Uart::enableInterrupts(void)
{
    // Register the interrupt handler
    InterruptHandler::getInstance().setInterruptHandler(this);

    // Enable the UART RX, TX and RX timeout interrupts
    UARTIntEnable(config_.base, UART_INT_RX | UART_INT_TX | UART_INT_RT);

    // Set the UART interrupt priority
    IntPrioritySet(config_.interrupt, (7 << 5));

    // Enable the UART interrupt
    IntEnable(config_.interrupt);
}

void Uart::disableInterrupts(void)
{
    // Disable the UART RX, TX and RX timeout interrupts
    UARTIntDisable(config_.base, UART_INT_RX | UART_INT_TX | UART_INT_RT);

    // Disable the UART interrupt
    IntDisable(config_.interrupt);
}

void Uart::rxLock(void)
{
    rxMutex_.take();
}

void Uart::txLock(void)
{
    txMutex_.take();
}

void Uart::rxUnlock(void)
{
    rxMutex_.give();
}

void Uart::txUnlock(void)
{
    txMutex_.give();
}

void Uart::rxUnlockFromInterrupt(void)
{
    rxMutex_.giveFromInterrupt();
}

void Uart::txUnlockFromInterrupt(void)
{
    txMutex_.giveFromInterrupt();
}

uint8_t Uart::readByte(void)
{
    int32_t byte;
    byte = UARTCharGetNonBlocking(config_.base);
    return (uint8_t)(byte & 0xFF);
}

uint32_t Uart::readByte(uint8_t * buffer, uint32_t length)
{
    uint32_t data;
    for (uint32_t i = 0; i < length; i++)
    {
        data = UARTCharGet(config_.base);
        *buffer++ = (uint8_t)data;
    }

    // Wait until it is complete
    while(UARTBusy(config_.base))
        ;

    return 0;
}

void Uart::writeByte(uint8_t byte)
{
    UARTCharPutNonBlocking(config_.base, byte);
}

uint32_t Uart::writeByte(uint8_t * buffer, uint32_t length)
{
    for (uint32_t i = 0; i < length; i++)
    {
        UARTCharPut(config_.base, *buffer++);
    }

    // Wait until it is complete
    while(UARTBusy(config_.base))
        ;

    return 0;
}

/*=============================== protected =================================*/

uint32_t Uart::getBase(void)
{
    return config_.base;
}

void Uart::interruptHandler(void)
{
    uint32_t status;

    // Read interrupt source
    status = UARTIntStatus(config_.base, true);

    // Clear UART interrupt in the NVIC
    IntPendClear(config_.interrupt);

    // Process TX interrupt
    if (status & UART_INT_TX)
    {
        UARTIntClear(config_.base, UART_INT_TX);
        interruptHandlerTx();
    }

    // Process RX interrupt
    if (status & UART_INT_RX ||
        status & UART_INT_RT)
    {
        UARTIntClear(config_.base, UART_INT_RX | UART_INT_RT);
        interruptHandlerRx();
    }
}

/*================================ private ==================================*/

void Uart::interruptHandlerRx(void)
{
    if (rx_callback_ != nullptr)
    {
        rx_callback_->execute();
    }
}

void Uart::interruptHandlerTx(void)
{
    if (tx_callback_ != nullptr)
    {
        tx_callback_->execute();
    }
}
