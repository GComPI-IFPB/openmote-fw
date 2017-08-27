/**
 * @file       At86rf215.cpp
 * @author     Pere Tuset-Peiro (peretuset@openmote.com)
 * @version    v0.1
 * @date       May, 2016
 * @brief
 *
 * @copyright  Copyright 2016, OpenMote Technologies, S.L.
 *             This file is licensed under the GNU General Public License v2.
 */

/*================================ include ==================================*/

#include "At86rf215.h"

#include "platform_types.h"

/*================================ define ===================================*/

/*================================ typedef ==================================*/

/*=============================== variables =================================*/

/*=============================== prototypes ================================*/

/*================================= public ==================================*/
At86rf215::At86rf215(Spi& spi, GpioOut& cs, GpioIn& gpio0):
    spi_(spi), cs_(cs), gpio0_(gpio0)
{
    cs_.high();
}

/*=============================== protected =================================*/

/*================================ private ==================================*/
