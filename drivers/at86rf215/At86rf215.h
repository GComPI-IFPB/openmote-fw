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
    At86rf215(Spi& spi, GpioOut& pwr, GpioOut& rst, GpioOut& cs, GpioIn& irq);
    void enable(void);
    void sleep(void);
    void wakeup(void);
    void on(void);
    void off(void);
    void reset(void);
    bool check(void);
    void setRxCallbacks(Callback* rxInit, Callback* rxDone);
    void setTxCallbacks(Callback* txInit, Callback* txDone);
    void enableInterrupts(void);
    void disableInterrupts(void);
private:
    Spi& spi_;
    GpioOut& pwr_;
    GpioOut& rst_;
    GpioOut& cs_;
    GpioIn& irq_;
};

#endif /* AT86RF215_H_ */
