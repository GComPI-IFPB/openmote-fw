/**
 * @file       Crc16.cpp
 * @author     Pere Tuset-Peiro (peretuset@openmote.com)
 * @version    v0.1
 * @date       February, 2019
 * @brief
 *
 * @copyright  Copyright 2019, OpenMote Technologies, S.L.
 *             This file is licensed under the GNU General Public License v2.
 */

/*================================ include ==================================*/

#include "Crc16.hpp"

#include "platform_includes.h"
#include "platform_types.h"

/*================================ define ===================================*/


#define SOC_ADC_ADCCON1_RCTRL0                  ( 0x00000004 )
#define SOC_ADC_ADCCON1_RCTRL1                  ( 0x00000008 )

/*================================ typedef ==================================*/

/*=============================== variables =================================*/

static const uint16_t crc_ok   = 0x0000;

/*=============================== prototypes ================================*/

/*================================= public ==================================*/

Crc16::Crc16(void):
  crc_(crc_seed)
{
}

void Crc16::init(void)
{
  /* CC2538 implements the CRC-16 BUYPASS with 0x8005 polynomial and 0x0000 seed */
  HWREG(SOC_ADC_RNDL) = 0x00;
  HWREG(SOC_ADC_RNDL) = 0x00;
}

uint16_t Crc16::get(void)
{
  crc_ = (HWREG(SOC_ADC_RNDH) << 8) | (HWREG(SOC_ADC_RNDL) << 0);
  return crc_;
}

void Crc16::set(uint8_t byte)
{
  HWREG(SOC_ADC_RNDH) = byte;
}

bool Crc16::check(void)
{
  crc_ = (HWREG(SOC_ADC_RNDH) << 8) | (HWREG(SOC_ADC_RNDL) << 0);
  return (crc_ == crc_ok);
}

/*=============================== protected =================================*/

/*================================ private ==================================*/
