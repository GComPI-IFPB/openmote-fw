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
}

uint8_t I2c::readByte(uint8_t address_, uint8_t register_)
{
    uint8_t data;

    I2CMasterSlaveAddrSet(address_, false);

    I2CMasterDataPut(register_);

    I2CMasterControl(I2C_MASTER_CMD_SINGLE_SEND);

    while(I2CMasterBusy())
        ;

    data = I2CMasterDataGet();

    I2CMasterSlaveAddrSet(address_, true);

    I2CMasterDataPut(register_);

    I2CMasterControl(I2C_MASTER_CMD_SINGLE_SEND);

    while(I2CMasterBusy())
        ;

    data = I2CMasterDataGet();
    
    return data;
}

uint8_t I2c::readByte(uint8_t address_, uint8_t register_, uint8_t * buffer, uint8_t size)
{
    return 0;
}

void I2c::writeByte(uint8_t address_, uint8_t register_, uint8_t data_)
{
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
