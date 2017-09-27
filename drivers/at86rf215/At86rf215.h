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
#include "Callback.h"

#include "At86rf215_regs.h"

class At86rf215 : public Radio
{
public:
    At86rf215(Spi& spi, GpioOut& pwr, GpioOut& rst, GpioOut& cs, GpioIn& irq);
    void enable(void);
    void sleep(void);
    void wakeup(void);
    void reset(void);
    bool check(void);
    void setCallbacks(Callback* rf09, Callback* r24);
    void clearCallbacks(void);
    void enableInterrupts(void);
    void disableInterrupts(void);
protected:
    void interruptHandler(void);    
private:
    void singleAccessRead(uint16_t address, uint8_t* value);
    void singleAccessWrite(uint16_t address, uint8_t value);
    void blockAccessRead(uint16_t address, uint8_t* values, uint16_t length);
    void blockAccessWrite(uint16_t address, uint8_t* values, uint16_t length);
private:
    Spi& spi_;
    GpioOut& pwr_;
    GpioOut& rst_;
    GpioOut& cs_;
    GpioIn& irq_;

    GenericCallback<At86rf215> callback_;

    Callback* rf09_cb;
    Callback* rf24_cb;
};

#endif /* AT86RF215_H_ */
