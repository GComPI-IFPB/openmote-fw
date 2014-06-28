#include "Board.h"

#include "gpio.h"
#include "interrupt.h"
#include "ioc.h"
#include "sys_ctrl.h"

#include "hw_gpio.h"
#include "hw_ints.h"
#include "hw_ioc.h"
#include "hw_sys_ctrl.h"

#include "hw_memmap.h"
#include "hw_types.h"

Board::Board()
{
    /**
     * Configure the 32 kHz pins, PD6 and PD7, for crystal operation
     * By default they are configured as GPIOs
     */
    GPIODirModeSet(GPIO_D_BASE, 0x40, GPIO_DIR_MODE_IN);
    GPIODirModeSet(GPIO_D_BASE, 0x80, GPIO_DIR_MODE_IN);
    IOCPadConfigSet(GPIO_D_BASE, 0x40, IOC_OVERRIDE_ANA);
    IOCPadConfigSet(GPIO_D_BASE, 0x80, IOC_OVERRIDE_ANA);

    /**
     * Set the real-time clock to use the 32.768 kHz external crystal
     * Set the system clock to use the internal 16 MHz internal crystal
     */
    SysCtrlClockSet(true, true, SYS_CTRL_SYSDIV_16MHZ);

    /**
     * Set the IO clock to operate at 16 MHz
     * This way peripherals can run while the system clock is gated
     */
    SysCtrlIOClockSet(SYS_CTRL_SYSDIV_16MHZ);
    
    /**
     * Wait until the 32 kHz oscillator becomes stable
     */
    while(HWREG(SYS_CTRL_CLOCK_STA) & SYS_CTRL_CLOCK_STA_OSC32K);
    
    runSetting();
    sleepSetting();
    deepSleepSetting();
}

void Board::reset(void)
{
    SysCtrlReset();
}

void Board::sleep(void)
{
    SysCtrlSleep();
}

void Board::setDeepSleepMode(uint32_t deepSleepMode_)
{
     SysCtrlPowerModeSet(deepSleepMode_);
}

void Board::deepSleep(void)
{
    SysCtrlDeepSleep();
}

void Board::enableInterrupts(void)
{
    IntMasterEnable();
}

void Board::disableInterrupts(void)
{
    IntMasterDisable();
}

void Board::runSetting(void)
{
    /* Disable General Purpose Timers 0, 1, 2, 3 when running */
    SysCtrlPeripheralDisable(SYS_CTRL_PERIPH_GPT0);
    SysCtrlPeripheralDisable(SYS_CTRL_PERIPH_GPT1);
    SysCtrlPeripheralDisable(SYS_CTRL_PERIPH_GPT2);
    SysCtrlPeripheralDisable(SYS_CTRL_PERIPH_GPT3);

    /* Disable SSI 0, 1 when running */
    SysCtrlPeripheralDisable(SYS_CTRL_PERIPH_SSI0);
    SysCtrlPeripheralDisable(SYS_CTRL_PERIPH_SSI1);

    /* Disable UART1 when running */
    SysCtrlPeripheralDisable(SYS_CTRL_PERIPH_UART0);
    SysCtrlPeripheralDisable(SYS_CTRL_PERIPH_UART1);

    /* Disable I2C, AES and PKA when running */
    SysCtrlPeripheralDisable(SYS_CTRL_PERIPH_I2C);
    SysCtrlPeripheralDisable(SYS_CTRL_PERIPH_PKA);
    SysCtrlPeripheralDisable(SYS_CTRL_PERIPH_AES);
    SysCtrlPeripheralDisable(SYS_CTRL_PERIPH_RFC);
}

void Board::sleepSetting(void)
{
    /* Disable General Purpose Timers 0, 1, 2, 3 during sleep */
    SysCtrlPeripheralSleepDisable(SYS_CTRL_PERIPH_GPT0);
    SysCtrlPeripheralSleepDisable(SYS_CTRL_PERIPH_GPT1);
    SysCtrlPeripheralSleepDisable(SYS_CTRL_PERIPH_GPT2);
    SysCtrlPeripheralSleepDisable(SYS_CTRL_PERIPH_GPT3);

    /* Disable SSI 0, 1 during sleep */
    SysCtrlPeripheralSleepDisable(SYS_CTRL_PERIPH_SSI0);
    SysCtrlPeripheralSleepDisable(SYS_CTRL_PERIPH_SSI1);

    /* Disable UART 0, 1 during sleep */
    SysCtrlPeripheralSleepDisable(SYS_CTRL_PERIPH_UART0);
    SysCtrlPeripheralSleepDisable(SYS_CTRL_PERIPH_UART1);

    /* Disable I2C, PKA, AES during sleep */
    SysCtrlPeripheralSleepDisable(SYS_CTRL_PERIPH_I2C);
    SysCtrlPeripheralSleepDisable(SYS_CTRL_PERIPH_PKA);
    SysCtrlPeripheralSleepDisable(SYS_CTRL_PERIPH_AES);
    SysCtrlPeripheralSleepDisable(SYS_CTRL_PERIPH_RFC);
}

void Board::deepSleepSetting(void)
{
    /* Disable General Purpose Timers 0, 1, 2, 3 during deep sleep */
    SysCtrlPeripheralDeepSleepDisable(SYS_CTRL_PERIPH_GPT0);
    SysCtrlPeripheralDeepSleepDisable(SYS_CTRL_PERIPH_GPT1);
    SysCtrlPeripheralDeepSleepDisable(SYS_CTRL_PERIPH_GPT2);
    SysCtrlPeripheralDeepSleepDisable(SYS_CTRL_PERIPH_GPT3);

    /* Disable SSI 0, 1 during deep sleep */
    SysCtrlPeripheralDeepSleepDisable(SYS_CTRL_PERIPH_SSI0);
    SysCtrlPeripheralDeepSleepDisable(SYS_CTRL_PERIPH_SSI1);

    /* Disable UART 0, 1 during deep sleep */
    SysCtrlPeripheralDeepSleepDisable(SYS_CTRL_PERIPH_UART0);
    SysCtrlPeripheralDeepSleepDisable(SYS_CTRL_PERIPH_UART1);

    /* Disable I2C, PKA, AES during deep sleep */
    SysCtrlPeripheralDeepSleepDisable(SYS_CTRL_PERIPH_I2C);
    SysCtrlPeripheralDeepSleepDisable(SYS_CTRL_PERIPH_PKA);
    SysCtrlPeripheralDeepSleepDisable(SYS_CTRL_PERIPH_AES);
    SysCtrlPeripheralDeepSleepDisable(SYS_CTRL_PERIPH_RFC);
}

