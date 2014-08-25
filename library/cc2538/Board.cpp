/*
 * Copyright 2013 OpenMote Technologies, S.L.
 */

/**
 *
 * @file       Board.cpp
 * @author     Pere Tuset-Peiro (peretuset@openmote.com)
 * @version    v0.1
 * @date       May, 2014
 * @brief
 * @ingroup
 *
 */

/*================================ include ==================================*/

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

/*================================ define ===================================*/

// Defines for the EUI64 address
#define CC2538_EUI64_ADDRESS_HI_H               ( 0x0028002F )
#define CC2538_EUI64_ADDRESS_HI_L               ( 0x0028002C )
#define CC2538_EUI64_ADDRESS_LO_H               ( 0x0028002B )
#define CC2538_EUI64_ADDRESS_LO_L               ( 0x00280028 )

/*================================ typedef ==================================*/

/*=============================== variables =================================*/

/*=============================== prototypes ================================*/

/*================================= public ==================================*/

Board::Board():
    sleepMode(SleepMode_None)
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
     * Set the system clock to use the 32 MHz external crystal divided to 16 MHz
     */
    SysCtrlClockSet(true, false, SYS_CTRL_SYSDIV_16MHZ);

    /**
     * Set the IO clock to operate at 16 MHz
     * This way peripherals can run while the system clock is gated
     */
    SysCtrlIOClockSet(SYS_CTRL_SYSDIV_16MHZ);

    /**
     * Wait until the 32 kHz oscillator becomes stable
     */
    while(HWREG(SYS_CTRL_CLOCK_STA) & SYS_CTRL_CLOCK_STA_OSC32K);

    /**
     * Wait until the 32 MHz oscillator becomes stable
     */
    while(HWREG(SYS_CTRL_CLOCK_STA) & !SYS_CTRL_CLOCK_STA_OSC);
}

void Board::reset(void)
{
    SysCtrlReset();
}

void Board::setSleepMode(SleepMode sleepMode_)
{
     sleepMode = sleepMode_;
}

void Board::sleep(void)
{
    if (sleepMode == SleepMode_None)
    {
        SysCtrlSleep();
    }
    else
    {
        SysCtrlPowerModeSet(sleepMode);
        SysCtrlDeepSleep();
    }
}

void Board::wakeup(void)
{
    if (sleepMode != SleepMode_None)
    {
        // Wake-up peripherals
    }
}

void Board::enableInterrupts(void)
{
    IntMasterEnable();
}

void Board::disableInterrupts(void)
{
    IntMasterDisable();
}

void Board::getAddress(uint8_t* address)
{
    uint8_t* eui64_flash;

    eui64_flash = (uint8_t*) CC2538_EUI64_ADDRESS_LO_H;
    while (eui64_flash >= (uint8_t*) CC2538_EUI64_ADDRESS_LO_L) {
        *address++ = *eui64_flash--;
    }

    eui64_flash = (uint8_t*) CC2538_EUI64_ADDRESS_HI_H;
    while (eui64_flash >= (uint8_t*) CC2538_EUI64_ADDRESS_HI_L) {
        *address++ = *eui64_flash--;
    }
}

/*=============================== protected =================================*/

/*================================ private ==================================*/
