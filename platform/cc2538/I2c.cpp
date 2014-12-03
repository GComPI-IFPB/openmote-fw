/*
 * Copyright 2013 OpenMote Technologies, S.L.
 */

/**
 *
 * @file       I2c.cpp
 * @author     Pere Tuset-Peiro (peretuset@openmote.com)
 * @version    v0.1
 * @date       May, 2014
 * @brief
 * @ingroup
 *
 */

/*================================ include ==================================*/

#include "I2c.h"

#include "cc2538_include.h"

/*================================ define ===================================*/

/*================================ typedef ==================================*/

/*=============================== variables =================================*/

/*=============================== prototypes ================================*/

/*================================= public ==================================*/

I2c::I2c(uint32_t peripheral, GpioI2c& scl, GpioI2c& sda):
    peripheral_(peripheral), scl_(scl), sda_(sda)
{
}

void I2c::enable(uint32_t clock)
{
    bool status;

    clock_ = clock;

    // Enable peripheral except in deep sleep modes (e.g. LPM1, LPM2, LPM3)
    SysCtrlPeripheralEnable(peripheral_);
    SysCtrlPeripheralSleepEnable(peripheral_);
    SysCtrlPeripheralDeepSleepDisable(peripheral_);

    // Reset peripheral previous to configuring it
    SysCtrlPeripheralReset(peripheral_);

    // Configure the SCL pin
    GPIOPinTypeI2C(scl_.getPort(), scl_.getPin());
    IOCPinConfigPeriphInput(scl_.getPort(), scl_.getPin(), IOC_I2CMSSCL);
    IOCPinConfigPeriphOutput(scl_.getPort(), scl_.getPin(), IOC_MUX_OUT_SEL_I2C_CMSSCL);

    // Configure the SDA pin
    GPIOPinTypeI2C(sda_.getPort(), sda_.getPin());
    IOCPinConfigPeriphInput(sda_.getPort(), sda_.getPin(), IOC_I2CMSSDA);
    IOCPinConfigPeriphOutput(sda_.getPort(), sda_.getPin(), IOC_MUX_OUT_SEL_I2C_CMSSDA);

    // Configure the I2C clock
    status = (clock_ == 400000 ? true : false);
    I2CMasterInitExpClk(SysCtrlClockGet(), status);

    // Enable the I2C module as master
    I2CMasterEnable();
}

void I2c::sleep(void)
{
    I2CMasterDisable();

    GPIOPinTypeGPIOOutput(scl_.getPort(), scl_.getPin());
    GPIOPinTypeGPIOOutput(sda_.getPort(), sda_.getPin());

    GPIOPinWrite(scl_.getPort(), scl_.getPin(), 0);
    GPIOPinWrite(sda_.getPort(), sda_.getPin(), 0);
}

void I2c::wakeup(void)
{
    enable(clock_);
}

bool I2c::readByte(uint8_t address, uint8_t* buffer)
{
    uint32_t delayTicks;

    I2CMasterSlaveAddrSet(address, true); // read

    I2CMasterControl(I2C_MASTER_CMD_SINGLE_RECEIVE);

    delayTicks = 10000;
    while(I2CMasterBusy() && delayTicks--) {
        if (delayTicks == 0) {
            return false;
        }
    }

    *buffer = I2CMasterDataGet();

    return true;
}

bool I2c::readByte(uint8_t address, uint8_t* buffer, uint8_t size)
{
    uint32_t delayTicks;

    I2CMasterSlaveAddrSet(address, true); // read

    I2CMasterControl(I2C_MASTER_CMD_BURST_RECEIVE_START);

    while(size) {
        delayTicks = 1000000;
        while(I2CMasterBusy() && delayTicks--) {
            if (delayTicks == 0) {
                return false;
            }
        }

        *buffer++ = I2CMasterDataGet();
        size--;

        if(size == 1) {
            I2CMasterControl(I2C_MASTER_CMD_BURST_RECEIVE_FINISH);
        } else {
            I2CMasterControl(I2C_MASTER_CMD_BURST_RECEIVE_CONT);
        }
    }

    return true;
}

bool I2c::writeByte(uint8_t address, uint8_t byte)
{
    uint32_t delayTicks;

    I2CMasterSlaveAddrSet(address, false); // write

    I2CMasterDataPut(byte);

    I2CMasterControl(I2C_MASTER_CMD_SINGLE_SEND);

    delayTicks = 10000;
    while(I2CMasterBusy() && delayTicks--) {
        if (delayTicks == 0) {
            return false;
        }
    }

    return true;
}

bool I2c::writeByte(uint8_t address, uint8_t* buffer, uint8_t size)
{
    uint32_t delayTicks;

    I2CMasterSlaveAddrSet(address, false); // write

    I2CMasterDataPut(*buffer++);
    size--;

    I2CMasterControl(I2C_MASTER_CMD_BURST_SEND_START);

    delayTicks = 1000000;
    while(I2CMasterBusy() && delayTicks--) {
        if (delayTicks == 0) {
            return false;
        }
    }

    while(size) {
        I2CMasterDataPut(*buffer++);
        size--;

        if (size == 0)
        {
            I2CMasterControl(I2C_MASTER_CMD_BURST_SEND_FINISH);
        }
        else
        {
            I2CMasterControl(I2C_MASTER_CMD_BURST_SEND_CONT);
        }

        delayTicks = 1000000;
        while(I2CMasterBusy() && delayTicks--) {
            if (delayTicks == 0) {
                return false;
            }
        }
    }

    return true;
}

/*=============================== protected =================================*/

void I2c::interruptHandler(void)
{
}

/*================================ private ==================================*/
