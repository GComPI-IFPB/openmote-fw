/*
 * Copyright 2013 OpenMote Technologies, S.L.
 */

/**
 *
 * @file       Rtc.h
 * @author     Pere Tuset-Peiro (peretuset@openmote.com)
 * @version    v0.1
 * @date       May, 2014
 * @brief
 * @ingroup
 *
 */

#ifndef RTC_H_
#define RTC_H_

#include <stdint.h>

#include "Callback.h"

class Rtc
{

friend class InterruptHandler;

public:
    Rtc(uint32_t interrupt_);
    void init(void);
    void start(uint32_t time);
    void stop(void);
    uint32_t read(void);
    void setCallback(Callback* callback_);
    void clearCallack(void);
    void enableInterrupt(void);
    void disableInterrupt(void);
protected:
    void interruptHandler(void);
private:
    uint32_t interrupt;

    Callback* callback;
};

#endif /* RTC_H_ */
