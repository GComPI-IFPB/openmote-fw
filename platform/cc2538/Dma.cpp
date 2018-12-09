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

#pragma data_alignment=1024
unsigned char ucDMAControlTable[1024];

/*=============================== prototypes ================================*/

/*================================= public ==================================*/

Dma::Dma()
{  
}

void Dma::init(void)
{
}

/*=============================== protected =================================*/

/*================================ private ==================================*/
