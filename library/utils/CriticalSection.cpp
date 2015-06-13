/**
 * @file       SemaphoreBinary.cpp
 * @author     Pere Tuset-Peiro (peretuset@openmote.com)
 * @version    v0.1
 * @date       May, 2015
 * @brief
 *
 * @copyright  Copyright 2015, OpenMote Technologies, S.L.
 *             This file is licensed under the GNU General Public License v2.
 */

/*================================ include ==================================*/

#include "CriticalSection.h"

/*================================ define ===================================*/

#define enterCriticalSection()              \
  do {                                      \
    asm (                                   \
        "MRS   R0, PRIMASK\n\t"             \
        "CPSID I\n\t"                       \
        "STRB R0, %[output]"                \
        : [output] "=m" (lock) :: "r0");   \
  } while(0)

#define exitCriticalSection()               \
  do{                                       \
    asm (                                   \
        "ldrb r0, %[input]\n\t"             \
        "msr PRIMASK,r0;\n\t"               \
        ::[input] "m" (lock) : "r0");      \
  } while(0)

/*================================ typedef ==================================*/

/*=============================== variables =================================*/

/*=============================== prototypes ================================*/

/*================================= public ==================================*/

CriticalSection::CriticalSection(void)
{
    enterCriticalSection();
}

CriticalSection::~CriticalSection(void)
{
    exitCriticalSection();
}

/*=============================== protected =================================*/

/*================================ private ==================================*/
