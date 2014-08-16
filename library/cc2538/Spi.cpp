/*
 * Copyright 2013 OpenMote Technologies, S.L.
 */

/**
 *
 * @file       Spi.cpp
 * @author     Pere Tuset-Peiro (peretuset@openmote.com)
 * @version    v0.1
 * @date       May, 2014
 * @brief
 * @ingroup
 *
 */

/*================================ include ==================================*/

#include "Spi.h"
#include "InterruptHandler.h"

#include "gpio.h"
#include "interrupt.h"
#include "ioc.h"
#include "sys_ctrl.h"
#include "ssi.h"

#include "hw_ioc.h"
#include "hw_ints.h"
#include "hw_types.h"
#include "hw_ssi.h"

/*================================ define ===================================*/

/*================================ typedef ==================================*/

/*=============================== variables =================================*/

/*=============================== prototypes ================================*/

/*================================= public ==================================*/

Spi::Spi(uint32_t peripheral_, uint32_t base_, uint32_t clock_, \
         GpioSpi& miso_, GpioSpi& mosi_, GpioSpi& clk_, GpioSpi& ncs_):
        peripheral(peripheral_), base(base_), clock(clock_), \
        miso(miso_), mosi(mosi_), clk(clk_), ncs(ncs_)
{
}

uint32_t Spi::getBase(void)
{
    return base;
}

void Spi::enable(uint32_t mode_, uint32_t protocol_, uint32_t datawidth_, uint32_t baudrate_)
{
    // Store SPI mode, protoco, baudrate and datawidth
    mode = mode_;
    protocol = protocol_;
    baudrate = baudrate_;
    datawidth = datawidth_;

    // Enable peripheral except in sleep and deep sleep modes
    SysCtrlPeripheralEnable(peripheral);
    SysCtrlPeripheralSleepDisable(peripheral);
    SysCtrlPeripheralDeepSleepDisable(peripheral);

    // Reset peripheral previous to configuring it
    SSIDisable(peripheral);

    // Set IO clock as SPI0 clock source
    SSIClockSourceSet(base, clock);

    // Configure the CLK, nCS, MOSI and MISO pins
    IOCPinConfigPeriphInput(miso.getPort(), miso.getPin(), miso.getIoc());
    IOCPinConfigPeriphOutput(mosi.getPort(), mosi.getPin(), mosi.getIoc());
    IOCPinConfigPeriphOutput(clk.getPort(), clk.getPin(), clk.getIoc());
    IOCPinConfigPeriphOutput(ncs.getPort(), ncs.getPin(), ncs.getIoc());

    // Configure SPI0 GPIOs
    GPIOPinTypeSSI(miso.getPort(), miso.getPin());
    GPIOPinTypeSSI(mosi.getPort(), mosi.getPin());
    GPIOPinTypeSSI(clk.getPort(), clk.getPin());
    GPIOPinTypeSSI(ncs.getPort(), ncs.getPin());

    // Configure the SPI0 clock
    SSIConfigSetExpClk(base, SysCtrlIOClockGet(), protocol, \
                       mode, SysCtrlClockGet()/2, datawidth);

    // Enable the SPI0 module
    SSIEnable(base);
}

void Spi::sleep(void)
{
    SSIDisable(base);

    GPIOPinTypeGPIOOutput(clk.getPort(), clk.getPin());
    GPIOPinTypeGPIOOutput(ncs.getPort(), ncs.getPin());
    GPIOPinTypeGPIOOutput(miso.getPort(), miso.getPin());
    GPIOPinTypeGPIOOutput(mosi.getPort(), mosi.getPin());

    GPIOPinWrite(clk.getPort(), clk.getPin(), 0);
    GPIOPinWrite(ncs.getPort(), ncs.getPin(), 0);
    GPIOPinWrite(miso.getPort(), miso.getPin(), 0);
    GPIOPinWrite(mosi.getPort(), mosi.getPin(), 0);
}

void Spi::wakeup(void)
{
    enable(mode, protocol, datawidth, baudrate);
}

void Spi::setRxCallback(Callback* callback_)
{
    rx_callback = callback_;
}

void Spi::setTxCallback(Callback* callback_)
{
    tx_callback = callback_;
}

void Spi::interruptEnable(void)
{
    // Register the interrupt handler
    InterruptHandler::getInstance().setInterruptHandler(this);

    // Enable the SPI interrupt
    SSIIntEnable(base, (SSI_TXFF | SSI_RXFF | SSI_RXTO | SSI_RXOR));

    // Enable the SPI interrupt
    IntEnable(interrupt);
}

void Spi::interruptDisable(void)
{
    // Disable the SPI interrupt
    SSIIntDisable(base, (SSI_TXFF | SSI_RXFF | SSI_RXTO | SSI_RXOR));

    // Disable the SPI interrupt
    IntDisable(interrupt);
}

uint8_t Spi::readByte(void)
{
    uint32_t byte;
    SSIDataGet(base, &byte);
    return (uint8_t)(byte & 0xFF);
}

uint32_t Spi::readByte(uint8_t* buffer, uint32_t length)
{
    uint32_t data;
    for (uint32_t i =  0; i < length; i++)
    {
        SSIDataGet(base, &data);
        *buffer++ = (uint8_t) data;

        // Wait until it is complete
        while(SSIBusy(base))
            ;
    }

    return 0;
}

void Spi::writeByte(uint8_t byte)
{
    SSIDataPut(base, byte);
}

uint32_t Spi::writeByte(uint8_t * buffer, uint32_t length)
{
    for (uint32_t i = 0; i < length; i++)
    {
        SSIDataPut(base, *buffer++);

        // Wait until it is complete
        while(SSIBusy(base))
            ;
    }

    return 0;
}

/*=============================== protected =================================*/

void Spi::interruptHandler(void)
{
    uint32_t status;

    // Read interrupt source
    status = SSIIntStatus(base, true);

    // Clear SPI interrupt in the NVIC
    IntPendClear(interrupt);

    // Process TX interrupt
    if (status & SSI_TXFF) {
        interruptHandlerTx();
    }

    // Process RX interrupt
    if (status & SSI_RXFF) {
        interruptHandlerRx();
    }
}

/*================================ private ==================================*/

void Spi::interruptHandlerRx(void)
{
    if (tx_callback != nullptr)
    {
        tx_callback->execute();
    }
}

void Spi::interruptHandlerTx(void)
{
    if (rx_callback != nullptr)
    {
        rx_callback->execute();
    }
}
