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

static void flashEraseCallback(void);

/*================================= public ==================================*/

Board::Board():
    sleepMode_(SleepMode_None), \
    flashEraseCallback_(&flashEraseCallback)
{
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
    for (volatile uint32_t i = 0xFFFF; i != 0; i--);
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

static void flashEraseCallback(void)
{
    IntMasterDisable();
    FlashMainPageErase(CC2538_FLASH_ADDRESS);
    SysCtrlReset();
}
