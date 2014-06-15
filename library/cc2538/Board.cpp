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
