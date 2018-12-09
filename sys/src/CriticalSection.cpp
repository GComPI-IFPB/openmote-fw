/**
 * @file       CriticalSection.cpp
 * @author     Pere Tuset-Peiro (peretuset@openmote.com)
 * @version    v0.1
 * @date       May, 2015
 * @brief
 *
 * @copyright  Copyright 2015, OpenMote Technologies, S.L.
 *             This file is licensed under the GNU General Public License v2.
 */

/*================================ include ==================================*/

#include "CriticalSection.hpp"

/*================================ define ===================================*/

/*================================ typedef ==================================*/

/*=============================== variables =================================*/

/*=============================== prototypes ================================*/

/*================================= public ==================================*/

CriticalSection::CriticalSection(void)
{
  taskENTER_CRITICAL();
}

CriticalSection::~CriticalSection(void)
{
  taskEXIT_CRITICAL();
}

/*=============================== protected =================================*/

/*================================ private ==================================*/

