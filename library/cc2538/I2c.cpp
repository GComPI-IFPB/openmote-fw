#include "I2c.h"
#include "Gpio.h"
#include "InterruptHandler.h"

I2c::I2c(uint32_t peripheral_, Gpio* scl_, Gpio* sda_):
    peripheral(peripheral_), scl(scl_), sda(sda_)
{
    SysCtrlPeripheralEnable(peripheral);
    SysCtrlPeripheralReset(peripheral);

    GPIOPinTypeI2C(scl->getPort(), scl->getPin());
    GPIOPinTypeI2C(sda->getPort(), sda->getPin());

    IOCPinConfigPeriphInput(scl->getPort(), scl->getPin(), IOC_I2CMSSCL);
    IOCPinConfigPeriphInput(sda->getPort(), sda->getPin(), IOC_I2CMSSDA);
    IOCPinConfigPeriphOutput(scl->getPort(), scl->getPin(), IOC_MUX_OUT_SEL_I2C_CMSSCL);
    IOCPinConfigPeriphOutput(sda->getPort(), sda->getPin(), IOC_MUX_OUT_SEL_I2C_CMSSDA);
}

void I2c::init(uint32_t clock_)
{
    bool status;

    status = (clock_ == 400000 ? true : false);
    
    I2CMasterEnable();

    I2CMasterInitExpClk(SysCtrlClockGet(), status);
    
    xMutex = xSemaphoreCreateMutex();
}

uint8_t I2c::readByte(uint8_t address_, uint8_t register_)
{
    uint8_t data;
    
    xSemaphoreTake(xMutex, portMAX_DELAY);

    I2CMasterSlaveAddrSet(address_, false); // write

    I2CMasterDataPut(register_);

    I2CMasterControl(I2C_MASTER_CMD_SINGLE_SEND);

    while(I2CMasterBusy())
        ;

    I2CMasterSlaveAddrSet(address_, true); // read

    I2CMasterControl(I2C_MASTER_CMD_SINGLE_RECEIVE);

    while(I2CMasterBusy())
        ;

    data = I2CMasterDataGet();
    
    xSemaphoreGive(xMutex);
    
    return data;
}

uint8_t I2c::readByte(uint8_t address_, uint8_t register_, uint8_t * buffer, uint8_t size)
{    
    xSemaphoreTake(xMutex, portMAX_DELAY);
    
    while(size) {

        I2CMasterSlaveAddrSet(address_, false); // write

        I2CMasterDataPut(register_++);

        I2CMasterControl(I2C_MASTER_CMD_SINGLE_SEND);

        while(I2CMasterBusy())
            ;

        I2CMasterSlaveAddrSet(address_, true); // read

        I2CMasterControl(I2C_MASTER_CMD_SINGLE_RECEIVE);

        while(I2CMasterBusy())
            ;

        *buffer++ = I2CMasterDataGet();
        size--;
    
    }
    
    xSemaphoreGive(xMutex);
    
    return size;
}

void I2c::writeByte(uint8_t address_, uint8_t register_)
{
    xSemaphoreTake(xMutex, portMAX_DELAY);
    
    I2CMasterSlaveAddrSet(address_, false); // write

    I2CMasterDataPut(register_);

    I2CMasterControl(I2C_MASTER_CMD_SINGLE_SEND);

    while(I2CMasterBusy())
        ;
    
    xSemaphoreGive(xMutex);
}

void I2c::writeByte(uint8_t address_, uint8_t register_, uint8_t data_)
{
    uint8_t buffer_[2] {register_, data_};
    writeByte(address_, buffer_, sizeof(buffer_));
}

void I2c::writeByte(uint8_t address_, uint8_t * data_, uint8_t size_)
{
    xSemaphoreTake(xMutex, portMAX_DELAY);
    
    I2CMasterSlaveAddrSet(address_, false); // write

    I2CMasterDataPut(*data_++);
    size_--;

    I2CMasterControl(I2C_MASTER_CMD_BURST_SEND_START);

    while(I2CMasterBusy())
        ;
        
    while(size_ > 1) {
        I2CMasterDataPut(*data_++);
        size_--;

        I2CMasterControl(I2C_MASTER_CMD_BURST_SEND_CONT);

        while(I2CMasterBusy())
            ;
    }
    
    I2CMasterDataPut(*data_);
    size_--;

    I2CMasterControl(I2C_MASTER_CMD_BURST_SEND_FINISH);

    while(I2CMasterBusy())
        ;
    
    xSemaphoreGive(xMutex);
}

void I2c::interruptEnable(void)
{
}

void I2c::interruptDisable(void)
{
}

void I2c::interruptHandler(void)
{
}
