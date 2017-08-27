/**
 * @file       At86rf215.h
 * @author     Pere Tuset-Peiro (peretuset@openmote.com)
 * @version    v0.1
 * @date       May, 2016
 * @brief
 *
 * @copyright  Copyright 2016, OpenMote Technologies, S.L.
 *             This file is licensed under the GNU General Public License v2.
 */

#ifndef AT86RF215_H_
#define AT86RF215_H_

#include "Radio.h"
#include "Gpio.h"
#include "Spi.h"

#include "At86rf215_regs.h"

class At86rf215 : public Radio
{
public:
    At86rf215(Spi& spi, GpioOut& cs, GpioIn& gpio0);
private:
    Spi& spi_;
    GpioOut& cs_;
    GpioIn& gpio0_;
};

#endif /* AT86RF215_H_ */
