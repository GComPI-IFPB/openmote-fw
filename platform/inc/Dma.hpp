/**
 * @file       Dma.hpp
 * @author     Pere Tuset-Peiro (peretuset@openmote.com)
 * @version    v0.1
 * @date       November, 2018
 * @brief
 *
 * @copyright  Copyright 2018, OpenMote Technologies, S.L.
 *             This file is licensed under the GNU General Public License v2.
 */

#ifndef DMA_HPP_
#define DMA_HPP_

#include <stdint.h>

#include "Callback.hpp"
#include "Mutex.hpp"

class Dma
{

friend class InterruptHandler;

public:
    Dma();
    void init(void);
private:
};

#endif /* DMA_HPP_ */
