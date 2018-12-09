/**
 * @file       Board.cpp
 * @author     Pere Tuset-Peiro (peretuset@openmote.com)
 * @version    v0.1
 * @date       November, 2018
 * @brief
 *
 * @copyright  Copyright 2018, OpenMote Technologies, S.L.
 *             This file is licensed under the GNU General Public License v2.
 */

/*================================ include ==================================*/

#include <string.h>

#include "Gpio.hpp"
#include "Board.hpp"

#include "platform_includes.h"
#include "platform_types.h"

/*================================ define ===================================*/

/* Defines for the EUI64 addresses in Flash */
#define CC2538_EUI64_ADDRESS_HI_H               ( 0x0028002F )
#define CC2538_EUI64_ADDRESS_HI_L               ( 0x0028002C )
#define CC2538_EUI64_ADDRESS_LO_H               ( 0x0028002B )
#define CC2538_EUI64_ADDRESS_LO_L               ( 0x00280028 )

/* Defines the CCA address in Flash */
#define CC2538_FLASH_CCA_ADDRESS                ( 0x0027F800 )

/*================================ typedef ==================================*/

/*=============================== variables =================================*/

extern GpioInPow button_user;

const uint32_t Board::BOARD_TICKS_PER_US = 31;

const uint32_t BOARD_BUTTON_DELAY_US    = 100000;
const uint32_t BOARD_BUTTON_DELAY_TICKS = (BOARD_BUTTON_DELAY_US / Board::BOARD_TICKS_PER_US);

/*=============================== prototypes ================================*/

static void flashEraseCallback(void);

/*================================= public ==================================*/

Board::Board():
    sleepMode_(SleepMode_None), \
    flashEraseCallback_(&flashEraseCallback)
{
}

void Board::init(BoardParams* params)
{ 
  /* Configure the 32 kHz clock pins, PD6 and PD7, for crystal operation */
  GPIODirModeSet(GPIO_D_BASE, GPIO_PIN_6, GPIO_DIR_MODE_IN);
  GPIODirModeSet(GPIO_D_BASE, GPIO_PIN_7, GPIO_DIR_MODE_IN);
  IOCPadConfigSet(GPIO_D_BASE, GPIO_PIN_6, IOC_OVERRIDE_ANA);
  IOCPadConfigSet(GPIO_D_BASE, GPIO_PIN_7, IOC_OVERRIDE_ANA);

  /* Set the system clocks */
  SysCtrlClockSet(params->bExternalOsc32k, params->bInternalOsc32M, params->SysClkDivider);

  /* Set the peripherals clock */
  SysCtrlIOClockSet(params->IOClkDivider);

  /* If using the 32 MHz crystal wait until it becomes stable */
  if (params->bInternalOsc32M == false)
  {
    while (!((HWREG(SYS_CTRL_CLOCK_STA)) & (SYS_CTRL_CLOCK_STA_XOSC_STB)))
      ;
  }

  /* If using the 32.768 kHz oscillator wait until it becomes stable */
  if (params->bExternalOsc32k == true)
  {
    while(HWREG(SYS_CTRL_CLOCK_STA) & SYS_CTRL_CLOCK_STA_SYNC_32K)
      ;
    while(!(HWREG(SYS_CTRL_CLOCK_STA) & SYS_CTRL_CLOCK_STA_SYNC_32K))
      ;
  }
}

void Board::reset(void)
{
  /* Force a software reset */
  SysCtrlReset();
}

uint32_t Board::getClock(void)
{
  return SysCtrlClockGet();
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
  /* Wake-up peripherals */
  if (sleepMode_ != SleepMode_None)
  {
    
  }
}

void Board::enableInterrupts(void)
{
  /* Enable global interrupts */
  IntMasterEnable();
}

void Board::disableInterrupts(void)
{
  /* Disable global interrupts */
  IntMasterDisable();
}

uint32_t Board::getCurrentTicks(void)
{
  /* Get the current number of ticks */
  return SleepModeTimerCountGet();
}

bool Board::isExpiredTicks(uint32_t future)
{
  uint32_t current;
  int32_t remaining;

  /* Get the current number of ticks */
  current = getCurrentTicks();

  /* Calculte the number of remaining ticks */
  remaining = (int32_t) (future - current);

  /* Returns true if it has expired */
  return (remaining < 0);
}

void Board::delayMilliseconds(uint32_t milliseconds)
{
  uint32_t future, current;

  current = getCurrentTicks();
  future = current + 32 * milliseconds;

  while(!isExpiredTicks(future))
      ;
}

void Board::enableFlashErase(void)
{
  uint32_t delayTicks = BOARD_BUTTON_DELAY_TICKS;

  /* Calculate timeout */
  delayTicks += getCurrentTicks();

  /* Wait until timeout */
  while (!isExpiredTicks(delayTicks));

  /* Set the callback and enable interrupt */
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
  /* Disable global interrupts */
  IntMasterDisable();

  /* Erase CCA page in Flash */
  FlashMainPageErase(CC2538_FLASH_CCA_ADDRESS);

  /* Force a software reset */
  SysCtrlReset();
}
