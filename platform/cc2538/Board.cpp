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

#include <string.h>

#include "Board.h"
#include "GpioInPow.h"

#include "cc2538_include.h"
#include "cc2538_types.h"

/*================================ define ===================================*/

// Defines for the EUI64 address
#define CC2538_EUI64_ADDRESS_HI_H               ( 0x0028002F )
#define CC2538_EUI64_ADDRESS_HI_L               ( 0x0028002C )
#define CC2538_EUI64_ADDRESS_LO_H               ( 0x0028002B )
#define CC2538_EUI64_ADDRESS_LO_L               ( 0x00280028 )

// Defines the Flash address
#define CC2538_FLASH_ADDRESS                    ( 0x0027F800 )

/*================================ typedef ==================================*/

/*=============================== variables =================================*/

extern GpioInPow button_user;

/*=============================== prototypes ================================*/

static void gpioInit(void);
static void clockInit(void);
static void flashEraseCallback(void);

/*================================= public ==================================*/

Board::Board():
    sleepMode_(SleepMode_None), \
    flashEraseCallback_(&flashEraseCallback)
{
    gpioInit();
    clockInit();
}

void Board::reset(void)
{
    SysCtrlReset();
}

void Board::setSleepMode(SleepMode sleepMode)
{
     sleepMode_ = sleepMode;
}

void Board::sleep(void)
{
    if (sleepMode_ == SleepMode_None)
    {
        SysCtrlSleep();
    }
    else
    {
        SysCtrlPowerModeSet(sleepMode_);
        SysCtrlDeepSleep();
    }
}

void Board::wakeup(void)
{
    if (sleepMode_ != SleepMode_None)
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

void Board::enableFlashErase(void)
{
    for(volatile uint32_t i = 0xFFFF; i != 0; i--);
    button_user.setCallback(&flashEraseCallback_);
    button_user.enableInterrupts();
}

void Board::getEUI48(uint8_t* address)
{
    uint8_t temp[8];

    getEUI64(temp);

    memcpy(&address[0], &temp[0], 3);
    memcpy(&address[3], &temp[5], 3);
}

void Board::getEUI64(uint8_t* address)
{
    uint8_t* eui64_flash;

    eui64_flash = (uint8_t*) CC2538_EUI64_ADDRESS_LO_H;
    while (eui64_flash >= (uint8_t*) CC2538_EUI64_ADDRESS_LO_L)
    {
        *address++ = *eui64_flash--;
    }

    eui64_flash = (uint8_t*) CC2538_EUI64_ADDRESS_HI_H;
    while (eui64_flash >= (uint8_t*) CC2538_EUI64_ADDRESS_HI_L)
    {
        *address++ = *eui64_flash--;
    }
}

/*=============================== protected =================================*/

/*================================ private ==================================*/

static void gpioInit(void)
{
    /**
     * Configure all pins as outputs
     */
    GPIOPinTypeGPIOOutput(GPIO_A_BASE, 0xFF);
    GPIOPinTypeGPIOOutput(GPIO_B_BASE, 0xFF);
    GPIOPinTypeGPIOOutput(GPIO_C_BASE, 0xFF);
    GPIOPinTypeGPIOOutput(GPIO_D_BASE, 0xFF);

    /**
     * Set all pins to low
     */
    GPIOPinWrite(GPIO_A_BASE, 0xFF, 0x00);
    GPIOPinWrite(GPIO_B_BASE, 0xFF, 0x00);
    GPIOPinWrite(GPIO_C_BASE, 0xFF, 0x00);
    GPIOPinWrite(GPIO_D_BASE, 0xFF, 0x00);
}

static void clockInit(void)
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
     * Set the system clock to use the 32 MHz external crystal
     */
    SysCtrlClockSet(true, false, SYS_CTRL_SYSDIV_32MHZ);

    /**
     * Set the IO clock to operate at 16 MHz
     * This way peripherals can run while the system clock is gated
     */
    SysCtrlIOClockSet(SYS_CTRL_SYSDIV_16MHZ);

    /**
     * Wait until the 32 MHz oscillator becomes stable
     */
    while (!((HWREG(SYS_CTRL_CLOCK_STA)) & (SYS_CTRL_CLOCK_STA_XOSC_STB)));
}

static void flashEraseCallback(void)
{
    IntMasterDisable();
    FlashMainPageErase(CC2538_FLASH_ADDRESS);
    SysCtrlReset();
}
