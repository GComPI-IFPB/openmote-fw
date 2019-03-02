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
#include "platform_types.hpp"

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

uint32_t Dma::memcpy(uint8_t* dst, uint8_t* src, uint32_t length)
{
  if (length < 1024)
  {
    /* Setup DMA control for memory copy */
    uDMAChannelControlSet(UDMA_CH30_SW | UDMA_PRI_SELECT, UDMA_SIZE_8 | UDMA_SRC_INC_8 | UDMA_DST_INC_8 | UDMA_ARB_8);
    
    /* Setup DMA buffers for memory copy using BASIC mode */
    uDMAChannelTransferSet(UDMA_CH30_SW | UDMA_PRI_SELECT, UDMA_MODE_AUTO, src, dst, length);
    
    /* Enable DMA channel */
    uDMAChannelEnable(UDMA_CH30_SW);
    
    /* Request DMA transfer */
    uDMAChannelRequest(UDMA_CH30_SW);
    
    /* Busy-wait until there are no more bytes to be copied */
    while(uDMAChannelSizeGet(UDMA_CH30_SW | UDMA_PRI_SELECT) > 0);
    
    /* Disable DMA channel */
    uDMAChannelAttributeDisable(UDMA_CH30_SW, UDMA_ATTR_ALL);
    
    return length;
  }
  
  return 0;
}

/*=============================== protected =================================*/

/*================================ private ==================================*/
