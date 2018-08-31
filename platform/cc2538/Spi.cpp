/**
 * @file       Spi.cpp
 * @author     Pere Tuset-Peiro (peretuset@openmote.com)
 * @version    v0.1
 * @date       May, 2015
 * @brief
 *
 * @copyright  Copyright 2015, OpenMote Technologies, S.L.
 *             This file is licensed under the GNU General Public License v2.
 */

/*================================ include ==================================*/

#include <Gpio.hpp>
#include <InterruptHandler.hpp>
#include <Spi.hpp>
#include "cc2538_include.h"
#include "platform_types.h"

/*================================ define ===================================*/

/*================================ typedef ==================================*/

/*=============================== variables =================================*/

/*=============================== prototypes ================================*/

/*================================= public ==================================*/

Spi::Spi(Gpio& miso, Gpio& mosi, Gpio& clk, SpiConfig& config):
        miso_(miso), mosi_(mosi), clk_(clk), config_(config)
{
}

void Spi::enable(uint32_t baudrate)
{
    GpioConfig& miso = miso_.getGpioConfig();
    GpioConfig& mosi = mosi_.getGpioConfig();
    GpioConfig& clk  = clk_.getGpioConfig();

    // Store baudrate in configuration
    if (baudrate != 0) {
        config_.baudrate = baudrate;
    }
    
    // Enable peripheral except in deep sleep modes (e.g. LPM1, LPM2, LPM3)
    SysCtrlPeripheralEnable(config_.peripheral);
    SysCtrlPeripheralSleepEnable(config_.peripheral);
    SysCtrlPeripheralDeepSleepDisable(config_.peripheral);

    // Reset peripheral previous to configuring it
    SSIDisable(config_.base);

    // Set IO clock as SPI0 clock source
    SSIClockSourceSet(config_.base, config_.clock);

    // Configure the MISO, MOSI, CLK and nCS pins as peripheral
    IOCPinConfigPeriphInput(miso.port, miso.pin, miso.ioc);
    IOCPinConfigPeriphOutput(mosi.port, mosi.pin, mosi.ioc);
    IOCPinConfigPeriphOutput(clk.port, clk.pin, clk.ioc);

    // Configure MISO, MOSI, CLK and nCS GPIOs
    GPIOPinTypeSSI(miso.port, miso.pin);
    GPIOPinTypeSSI(mosi.port, mosi.pin);
    GPIOPinTypeSSI(clk.port, clk.pin);

    // Configure the SPI0 clock
    SSIConfigSetExpClk(config_.base, SysCtrlIOClockGet(), config_.protocol, \
                       config_.mode, config_.baudrate, config_.datawidth);

    // Enable the SPI0 module
    SSIEnable(config_.base);
}

void Spi::sleep(void)
{
    GpioConfig& miso = miso_.getGpioConfig();
    GpioConfig& mosi = mosi_.getGpioConfig();
    GpioConfig& clk  = clk_.getGpioConfig();

    SSIDisable(config_.base);

    // Configure the MISO, MOSI and CLK pins as output
    GPIOPinTypeGPIOInput(miso.port, miso.pin);
    GPIOPinTypeGPIOInput(mosi.port, mosi.pin);
    GPIOPinTypeGPIOInput(clk.port, clk.pin);
}

void Spi::wakeup(void)
{
    enable();
}

void Spi::setRxCallback(Callback* callback)
{
    rx_callback_ = callback;
}

void Spi::setTxCallback(Callback* callback)
{
    tx_callback_ = callback;
}

void Spi::enableInterrupts(void)
{
    // Register the interrupt handler
    InterruptHandler::getInstance().setInterruptHandler(this);

    // Enable the SPI interrupt
    SSIIntEnable(config_.base, (SSI_TXFF | SSI_RXFF | SSI_RXTO | SSI_RXOR));

    // Enable the SPI interrupt
    IntEnable(config_.interrupt);
}

void Spi::disableInterrupts(void)
{
    // Disable the SPI interrupt
    SSIIntDisable(config_.base, (SSI_TXFF | SSI_RXFF | SSI_RXTO | SSI_RXOR));

    // Disable the SPI interrupt
    IntDisable(config_.interrupt);
}

uint8_t Spi::rwByte(uint8_t byte)
{
    uint32_t ret;

    // Push a byte
    SSIDataPut(config_.base, byte);

    // Wait until it is complete
    while(SSIBusy(config_.base))
        ;

    // Read a byte
    SSIDataGet(config_.base, &ret);

    return (uint8_t)(ret & 0xFF);
}

bool Spi::rwByte(uint8_t* readBuffer, uint32_t readLength, uint8_t* writeBuffer, uint32_t writeLength)
{
    uint32_t data;

    if ((readLength == 0) || (writeLength == 0) || (readLength != writeLength))
    {
        return false;
    }

    for (uint32_t i =  0; i < readLength; i++)
    {
        // Push a byte
        SSIDataPut(config_.base, readBuffer[i]);

        // Wait until it is complete
        while(SSIBusy(config_.base))
            ;

        // Read a byte
        SSIDataGet(config_.base, &data);

        // Store the result
        writeBuffer[i] = (uint8_t)(data & 0xFF);
    }

    return true;
}

/*=============================== protected =================================*/

SpiConfig& Spi::getConfig(void)
{
    return config_;
}

void Spi::interruptHandler(void)
{
    uint32_t status;

    // Read interrupt source
    status = SSIIntStatus(config_.base, true);

    // Clear SPI interrupt in the NVIC
    IntPendClear(config_.interrupt);

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
    if (tx_callback_ != nullptr)
    {
        tx_callback_->execute();
    }
}

void Spi::interruptHandlerTx(void)
{
    if (rx_callback_ != nullptr)
    {
        rx_callback_->execute();
    }
}
