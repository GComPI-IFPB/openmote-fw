/**
 * @file       Dma.cpp
 * @author     Pere Tuset-Peiro (peretuset@openmote.com)
 * @version    v0.1
 * @date       November, 2018
 * @brief
 *
 * @copyright  Copyright 2018, OpenMote Technologies, S.L.
 *             This file is licensed under the GNU General Public License v2.
 */

/*================================ include ==================================*/

#include "Board.hpp"
#include "Gpio.hpp"
#include "Dma.hpp"

#include "platform_includes.h"
#include "platform_types.h"

/*================================ define ===================================*/

/*================================ typedef ==================================*/

/*=============================== variables =================================*/

#if defined(__IAR_SYSTEMS_ICC__)
#pragma data_alignment=1024
static unsigned char ucDMAControlTable[1024];
#else
static unsigned char ucDMAControlTable[1024] __attribute__ ((aligned(1024)));
#endif

/*=============================== prototypes ================================*/

/*================================= public ==================================*/

Dma::Dma():
  is_initialized(false)
{  
}

void Dma::init(void)
{
  if(!is_initialized)
  {
    /* Enable the uDMA controller */
    uDMAEnable();

    /* Set the base for the channel control table */
    uDMAControlBaseSet(&ucDMAControlTable[0]);
    
    /* Mark as initialized */
    is_initialized = true;
  }
}

/*=============================== protected =================================*/

/*================================ private ==================================*/
