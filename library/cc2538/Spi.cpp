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

/**********************************include************************************/

#include "Spi.h"

#include "gpio.h"
#include "ioc.h"
#include "sys_ctrl.h"
#include "ssi.h"
#include "interrupt.h"

#include "hw_ioc.h"
#include "hw_ints.h"
#include "hw_types.h"
#include "hw_ssi.h"

/**********************************defines************************************/



/*********************************variables***********************************/



/**********************************public*************************************/

Spi::Spi(uint32_t peripheral_, uint32_t base_, uint32_t clock_, \
         Gpio* miso_, Gpio* mosi_, Gpio* clk_, Gpio* ncs_):
        peripheral(peripheral_), base(base_), clock(clock_), \
        miso(miso_), mosi(mosi_), clk(clk_), ncs(ncs_)
{
}

uint32_t Spi::getBase(void)
{
    return base;
}

void Spi::enable(uint32_t mode_, uint32_t protocol_, uint32_t baudrate_, uint32_t config_)
{
    mode = mode_;
    protocol = protocol_;
    baudrate = baudrate_;
    config = config_;

    // Enable peripheral except in sleep and deep sleep modes
    SysCtrlPeripheralEnable(peripheral);
    SysCtrlPeripheralSleepDisable(peripheral);
    SysCtrlPeripheralDeepSleepDisable(peripheral);

    // Reset peripheral previous to configuring it
    SSIDisable(peripheral);

    // Set IO clock as SPI0 clock source
    SSIClockSourceSet(base, clock);

    // Configure the CLK, nCS, MOSI and MISO pins
    IOCPinConfigPeriphOutput(clk->getPort(), clk->getPin(), IOC_MUX_OUT_SEL_SSI0_CLKOUT);
    IOCPinConfigPeriphOutput(ncs->getPort(), ncs->getPin(), IOC_MUX_OUT_SEL_SSI0_FSSOUT);
    IOCPinConfigPeriphOutput(mosi->getPort(), mosi->getPin(), IOC_MUX_OUT_SEL_SSI0_TXD);
    IOCPinConfigPeriphInput(miso->getPort(), miso->getPin(), IOC_SSIRXD_SSI0);

    // Configure SPI0 GPIOs
    GPIOPinTypeSSI(base, clk->getPin() | ncs->getPin() | mosi->getPin() | miso->getPin());

    // Configure the SPI0 clock
    SSIConfigSetExpClk(base, SysCtrlIOClockGet(), SSI_FRF_MOTO_MODE_3,
                       mode, SysCtrlClockGet()/2, 8);

    // Enable the SPI0 module
    SSIEnable(base);
}

void Spi::sleep(void)
{
    SSIDisable(base);

    GPIOPinTypeGPIOOutput(clk->getPort(), clk->getPin());
    GPIOPinTypeGPIOOutput(ncs->getPort(), ncs->getPin());
    GPIOPinTypeGPIOOutput(miso->getPort(), miso->getPin());
    GPIOPinTypeGPIOOutput(mosi->getPort(), mosi->getPin());

    GPIOPinWrite(clk->getPort(), clk->getPin(), 0);
    GPIOPinWrite(ncs->getPort(), ncs->getPin(), 0);
    GPIOPinWrite(miso->getPort(), miso->getPin(), 0);
    GPIOPinWrite(mosi->getPort(), mosi->getPin(), 0);
}

void Spi::wakeup(void)
{
    enable(mode, protocol, baudrate, config);
}

void Spi::setRxCallback(callback_t callback_)
{
    rx_callback = callback_;
}

void Spi::setTxCallback(callback_t callback_)
{
    tx_callback = callback_;
}

void Spi::interruptEnable(void)
{
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

uint8_t Spi::readByte(uint8_t* buffer, uint32_t length)
{
    uint32_t data;
    for (uint32_t i =  0; i < length; i++)
    {
        SSIDataGet(base, &data);
        *buffer++ = (uint8_t) data;
    }

    // Wait until it is complete
    while(SSIBusy(base))
        ;
}

void Spi::writeByte(uint8_t byte)
{
    SSIDataPut(base, byte);
}

void Spi::writeByte(uint8_t * buffer, uint32_t length)
{
    for (uint32_t i = 0; i < length; i++)
    {
        SSIDataPut(base, *buffer++);
    }

    // Wait until it is complete
    while(SSIBusy(base))
        ;

}

/*********************************protected***********************************/



/**********************************private************************************/

void Spi::interruptHandler(void)
{
}
