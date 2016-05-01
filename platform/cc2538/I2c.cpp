/**
 * @file       I2c.cpp
 * @author     Pere Tuset-Peiro (peretuset@openmote.com)
 * @version    v0.1
 * @date       May, 2015
 * @brief
 *
 * @copyright  Copyright 2015, OpenMote Technologies, S.L.
 *             This file is licensed under the GNU General Public License v2.
 */

/*================================ include ==================================*/

#include "Board.h"
#include "Gpio.h"
#include "I2c.h"

#include "cc2538_include.h"
#include "platform_types.h"

/*================================ define ===================================*/

/*================================ typedef ==================================*/

/*=============================== variables =================================*/

extern Board board;

const uint32_t I2C_MAX_DELAY_US    = 100000;
const uint32_t I2C_MAX_DELAY_TICKS = I2C_MAX_DELAY_US / Board::BOARD_TICKS_PER_US;

/*=============================== prototypes ================================*/

/*================================= public ==================================*/

I2c::I2c(Gpio& scl, Gpio& sda, I2cConfig& config):
    scl_(scl), sda_(sda), config_(config)
{
}

void I2c::enable(uint32_t baudrate)
{
    bool status;

    // Store baudrate in configuration
    if (baudrate != 0) {
        config_.baudrate = baudrate;
    }

    GpioConfig& scl = scl_.getGpioConfig();
    GpioConfig& sda = sda_.getGpioConfig();

    // Enable peripheral except in deep sleep modes (e.g. LPM1, LPM2, LPM3)
    SysCtrlPeripheralEnable(config_.peripheral);
    SysCtrlPeripheralSleepEnable(config_.peripheral);
    SysCtrlPeripheralDeepSleepDisable(config_.peripheral);

    // Reset peripheral previous to configuring it
    SysCtrlPeripheralReset(config_.peripheral);

    // Configure the SCL pin
    GPIOPinTypeI2C(scl.port, scl.pin);
    IOCPinConfigPeriphInput(scl.port, scl.pin, IOC_I2CMSSCL);
    IOCPinConfigPeriphOutput(scl.port, scl.pin, IOC_MUX_OUT_SEL_I2C_CMSSCL);

    // Configure the SDA pin
    GPIOPinTypeI2C(sda.port, sda.pin);
    IOCPinConfigPeriphInput(sda.port, sda.pin, IOC_I2CMSSDA);
    IOCPinConfigPeriphOutput(sda.port, sda.pin, IOC_MUX_OUT_SEL_I2C_CMSSDA);

    // Configure the I2C clock
    status = ((config_.baudrate == 100000) ? false : true);
    I2CMasterInitExpClk(SysCtrlClockGet(), status);

    // Enable the I2C module as master
    I2CMasterEnable();
}

void I2c::sleep(void)
{
    GpioConfig& scl = scl_.getGpioConfig();
    GpioConfig& sda = sda_.getGpioConfig();

    // Disable the I2C module
    I2CMasterDisable();

    // Configure GPIOs as output
    GPIOPinTypeGPIOOutput(scl.port, scl.pin);
    GPIOPinTypeGPIOOutput(sda.port, sda.pin);

    // Set GPIOs to low
    GPIOPinWrite(scl.port, scl.pin, 0);
    GPIOPinWrite(sda.port, sda.pin, 0);
}

void I2c::wakeup(void)
{
    // Reenable the I2C peripheral
    enable();
}

void I2c::lock(void)
{
    semaphore_.take();
}

void I2c::unlock(void)
{
    semaphore_.give();
}

void I2c::unlockFromInterrupt(void)
{
    semaphore_.giveFromInterrupt();
}

bool I2c::readByte(uint8_t address, uint8_t* buffer)
{
    uint32_t delayTicks = I2C_MAX_DELAY_TICKS;

    // Read operation
    I2CMasterSlaveAddrSet(address, true);

    // Single read operation
    I2CMasterControl(I2C_MASTER_CMD_SINGLE_RECEIVE);

    // Calculate timeout
    delayTicks += board.getCurrentTicks();

    // Wait until complete or timeout
    while (I2CMasterBusy())
    {
        // Check timeout status and return if expired
        if (board.isExpiredTicks(delayTicks)) return false;
    }

    // Read data from I2C
    *buffer = I2CMasterDataGet();

    return true;
}

bool I2c::readByte(uint8_t address, uint8_t* buffer, uint8_t size)
{
    uint32_t delayTicks = I2C_MAX_DELAY_TICKS;

    // Read operation
    I2CMasterSlaveAddrSet(address, true);

    // Burst read operation
    I2CMasterControl(I2C_MASTER_CMD_BURST_RECEIVE_START);

    // Calculate timeout
    delayTicks += board.getCurrentTicks();

    // Iterate overall all bytes
    while (size)
    {
        // Wait until complete or timeout
        while (I2CMasterBusy())
        {
            // Check timeout status and return if expired
            if (board.isExpiredTicks(delayTicks)) return false;
        }

        // Read data from I2C
        *buffer++ = I2CMasterDataGet();
        size--;

        // Check if it's the last byte
        if (size == 1) I2CMasterControl(I2C_MASTER_CMD_BURST_RECEIVE_FINISH);
        else           I2CMasterControl(I2C_MASTER_CMD_BURST_RECEIVE_CONT);
    }

    return true;
}

bool I2c::writeByte(uint8_t address, uint8_t byte)
{
    uint32_t delayTicks = I2C_MAX_DELAY_TICKS;

    // Write operation
    I2CMasterSlaveAddrSet(address, false);

    // Write data to I2C
    I2CMasterDataPut(byte);

    // Single write operation
    I2CMasterControl(I2C_MASTER_CMD_SINGLE_SEND);

    // Calculate timeout
    delayTicks += board.getCurrentTicks();

    // Wait until complete or timeout
    while (I2CMasterBusy())
    {
        // Check timeout status and return if expired
        if (board.isExpiredTicks(delayTicks)) return false;
    }

    return true;
}

bool I2c::writeByte(uint8_t address, uint8_t* buffer, uint8_t size)
{
    uint32_t delayTicks = I2C_MAX_DELAY_TICKS;

    // Write operation
    I2CMasterSlaveAddrSet(address, false);

    // Write data to I2C
    I2CMasterDataPut(*buffer++);
    size--;

    // Burst write operation
    I2CMasterControl(I2C_MASTER_CMD_BURST_SEND_START);

    // Calculate timeout
    delayTicks += board.getCurrentTicks();

    // Wait until complete or timeout
    while (I2CMasterBusy())
    {
        // Check timeout status and return if expired
        if (board.isExpiredTicks(delayTicks)) return false;
    }

    while (size)
    {
        // Write data to I2C
        I2CMasterDataPut(*buffer++);
        size--;

        // Check if it's the last byte
        if (size == 0) I2CMasterControl(I2C_MASTER_CMD_BURST_SEND_FINISH);
        else           I2CMasterControl(I2C_MASTER_CMD_BURST_SEND_CONT);

        // Wait until complete or timeout
        while (I2CMasterBusy())
        {
            // Check timeout status and return if expired
            if (board.isExpiredTicks(delayTicks)) return false;
        }
    }

    return true;
}

/*=============================== protected =================================*/

void I2c::interruptHandler(void)
{
}

/*================================ private ==================================*/
