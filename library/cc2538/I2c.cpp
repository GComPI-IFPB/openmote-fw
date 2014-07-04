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
#include "Gpio.h"
#include "InterruptHandler.h"

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
    
    xMutex = xSemaphoreCreateMutex();
    if (xMutex == NULL) {
        
    }
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

void I2c::lock(void)
{
    xSemaphoreTake(xMutex, portMAX_DELAY);
}

void I2c::unlock(void)
{
    xSemaphoreGive(xMutex);
}

uint8_t I2c::readByte(uint8_t address_, uint8_t register_)
{
    uint8_t data;
    
    I2CMasterSlaveAddrSet(address_, false); // write

    I2CMasterDataPut(register_);

    I2CMasterControl(I2C_MASTER_CMD_SINGLE_SEND);
    
    TickType_t delayMilliseconds = 10;
    while(I2CMasterBusy() && delayMilliseconds--) {
        vTaskDelay(1 / portTICK_PERIOD_MS);
    }
        
    if (delayMilliseconds == 0) {
        xSemaphoreGive(xMutex);
        return 0;
    }

    I2CMasterSlaveAddrSet(address_, true); // read

    I2CMasterControl(I2C_MASTER_CMD_SINGLE_RECEIVE);

    delayMilliseconds = 10;
    while(I2CMasterBusy() && delayMilliseconds--) {
        vTaskDelay(1 / portTICK_PERIOD_MS);
    }
    
    if (delayMilliseconds == 0) {
        xSemaphoreGive(xMutex);
        return 0;
    }

    data = I2CMasterDataGet();
    
    return data;
}

uint8_t I2c::readByte(uint8_t address_, uint8_t register_, uint8_t * buffer, uint8_t size)
{        
    while(size) {

        I2CMasterSlaveAddrSet(address_, false); // write

        I2CMasterDataPut(register_++);

        I2CMasterControl(I2C_MASTER_CMD_SINGLE_SEND);

        TickType_t delayMilliseconds = 10;
        while(I2CMasterBusy() && delayMilliseconds--) {
            vTaskDelay(1 / portTICK_PERIOD_MS);
        }
        
        if (delayMilliseconds == 0) {
            xSemaphoreGive(xMutex);
            return 0;
        }

        I2CMasterSlaveAddrSet(address_, true); // read

        I2CMasterControl(I2C_MASTER_CMD_SINGLE_RECEIVE);

        delayMilliseconds = 10;
        while(I2CMasterBusy() && delayMilliseconds--) {
            vTaskDelay(1 / portTICK_PERIOD_MS);
        }
        
        if (delayMilliseconds == 0) {
            xSemaphoreGive(xMutex);
            return 0;
        }

        *buffer++ = I2CMasterDataGet();
        size--;
    }
    
    return size;
}

void I2c::writeByte(uint8_t address_, uint8_t register_)
{    
    I2CMasterSlaveAddrSet(address_, false); // write

    I2CMasterDataPut(register_);

    I2CMasterControl(I2C_MASTER_CMD_SINGLE_SEND);

    TickType_t delayMilliseconds = 10;
    while(I2CMasterBusy() && delayMilliseconds--) {
        vTaskDelay(1 / portTICK_PERIOD_MS);
    }
    
    if (delayMilliseconds == 0) {
        xSemaphoreGive(xMutex);
        return;
    }
}

void I2c::writeByte(uint8_t address_, uint8_t register_, uint8_t data_)
{
    uint8_t buffer_[2] {register_, data_};
    writeByte(address_, buffer_, sizeof(buffer_));
}

void I2c::writeByte(uint8_t address_, uint8_t * data_, uint8_t size_)
{
    I2CMasterSlaveAddrSet(address_, false); // write

    I2CMasterDataPut(*data_++);
    size_--;

    I2CMasterControl(I2C_MASTER_CMD_BURST_SEND_START);

    TickType_t delayMilliseconds = 10;
    while(I2CMasterBusy() && delayMilliseconds--) {
        vTaskDelay(1 / portTICK_PERIOD_MS);
    }
    
    if (delayMilliseconds == 0) {
        xSemaphoreGive(xMutex);
        return;
    }
        
    while(size_ > 1) {
        I2CMasterDataPut(*data_++);
        size_--;

        I2CMasterControl(I2C_MASTER_CMD_BURST_SEND_CONT);

        delayMilliseconds = 10;
        while(I2CMasterBusy() && delayMilliseconds--) {
            vTaskDelay(1 / portTICK_PERIOD_MS);
        }
        
        if (delayMilliseconds == 0) {
            xSemaphoreGive(xMutex);
            return;
        }
    }
    
    I2CMasterDataPut(*data_);
    size_--;

    I2CMasterControl(I2C_MASTER_CMD_BURST_SEND_FINISH);

    delayMilliseconds = 10;
    while(I2CMasterBusy() && delayMilliseconds--) {
        vTaskDelay(1 / portTICK_PERIOD_MS);
    }
    
    if (delayMilliseconds == 0) {
        xSemaphoreGive(xMutex);
        return;
    }   
}

/*********************************protected***********************************/

void I2c::interruptHandler(void)
{
}

/**********************************private************************************/

