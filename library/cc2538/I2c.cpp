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

/**********************************include************************************/

#include "I2c.h"

#include "gpio.h"
#include "interrupt.h"
#include "ioc.h"
#include "i2c.h"
#include "sys_ctrl.h"

#include "hw_ioc.h"
#include "hw_ints.h"
#include "hw_types.h"
#include "hw_i2cm.h"
#include "hw_i2cs.h"

/*********************************variables***********************************/

#define DELAY_TICKS         ( )

/**********************************public*************************************/

I2c::I2c(uint32_t peripheral_, Gpio* scl_, Gpio* sda_):
    peripheral(peripheral_), scl(scl_), sda(sda_)
{
}

void I2c::enable(uint32_t clock_)
{
    bool status;
    
    clock = clock_;
    
    SysCtrlPeripheralEnable(peripheral);
    SysCtrlPeripheralSleepDisable(peripheral);
    SysCtrlPeripheralDeepSleepDisable(peripheral);
    
    SysCtrlPeripheralReset(peripheral);

    GPIOPinTypeI2C(scl->getPort(), scl->getPin());
    IOCPinConfigPeriphInput(scl->getPort(), scl->getPin(), IOC_I2CMSSCL);
    IOCPinConfigPeriphOutput(scl->getPort(), scl->getPin(), IOC_MUX_OUT_SEL_I2C_CMSSCL);

    GPIOPinTypeI2C(sda->getPort(), sda->getPin());    
    IOCPinConfigPeriphInput(sda->getPort(), sda->getPin(), IOC_I2CMSSDA);
    IOCPinConfigPeriphOutput(sda->getPort(), sda->getPin(), IOC_MUX_OUT_SEL_I2C_CMSSDA);
    
    status = (clock == 400000 ? true : false);
    
    I2CMasterEnable();

    I2CMasterInitExpClk(SysCtrlClockGet(), status);
}

void I2c::sleep(void)
{
    I2CMasterDisable();
    
    GPIOPinTypeGPIOOutput(scl->getPort(), scl->getPin());
    GPIOPinTypeGPIOOutput(sda->getPort(), sda->getPin());
    
    GPIOPinWrite(scl->getPort(), scl->getPin(), 0);
    GPIOPinWrite(sda->getPort(), sda->getPin(), 0);
}

void I2c::wakeup(void)
{
    bool status;
    
    SysCtrlPeripheralReset(peripheral);

    GPIOPinTypeI2C(scl->getPort(), scl->getPin());
    IOCPinConfigPeriphInput(scl->getPort(), scl->getPin(), IOC_I2CMSSCL);
    IOCPinConfigPeriphOutput(scl->getPort(), scl->getPin(), IOC_MUX_OUT_SEL_I2C_CMSSCL);

    GPIOPinTypeI2C(sda->getPort(), sda->getPin());    
    IOCPinConfigPeriphInput(sda->getPort(), sda->getPin(), IOC_I2CMSSDA);
    IOCPinConfigPeriphOutput(sda->getPort(), sda->getPin(), IOC_MUX_OUT_SEL_I2C_CMSSDA);
    
    status = (clock == 400000 ? true : false);
    
    I2CMasterEnable();

    I2CMasterInitExpClk(SysCtrlClockGet(), status);
}

bool I2c::readByte(uint8_t address_, uint8_t * buffer)
{
    uint32_t delayTicks;
    
    I2CMasterSlaveAddrSet(address_, true); // read

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

bool I2c::readByte(uint8_t address_, uint8_t * buffer, uint8_t size)
{
    uint32_t delayTicks;
    
    I2CMasterSlaveAddrSet(address_, true); // read

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

bool I2c::writeByte(uint8_t address_, uint8_t register_)
{
    uint32_t delayTicks;
    
    I2CMasterSlaveAddrSet(address_, false); // write

    I2CMasterDataPut(register_);

    I2CMasterControl(I2C_MASTER_CMD_SINGLE_SEND);

    delayTicks = 10000;
    while(I2CMasterBusy() && delayTicks--) {
        if (delayTicks == 0) {
            return false;
        }
    }
    
    return true;
}

/*********************************protected***********************************/

void I2c::interruptHandler(void)
{
}

/**********************************private************************************/

