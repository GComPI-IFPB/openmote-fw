/*
 * Copyright 2013 OpenMote Technologies, S.L.
 */

/**
 *
 * @file       RadioTimer.h
 * @author     Pere Tuset-Peiro (peretuset@openmote.com)
 * @version    v0.1
 * @date       May, 2014
 * @brief
 * @ingroup
 *
 */

#ifndef RADIO_TIMER_H_
#define RADIO_TIMER_H_

#include <stdint.h>

#include "Callback.h"

class RadioTimer
{

friend class InterruptHandler;

public:
    RadioTimer(uint32_t interrupt);
    void init(void);
    void start(void);
    void stop(void);
    uint32_t getCounter(void);
    void setCounter(uint32_t counter);
    uint32_t getPeriod(void);
    void setPeriod(uint32_t period);
    uint32_t getCapture(void);
    void setCapture(uint32_t capture_);
    void setCaptureCallback(Callback* capture);
    void setCompareCallback(Callback* compare);
    void setOverflowCallback(Callback* overflow);
    void enableInterrupts(void);
    void disableInterrupts(void);
protected:
    void interruptHandler(void);
private:
    Callback* overflow_;
    Callback* capture_;
    Callback* compare_;

    uint32_t interrupt_;
};

#endif /* RADIO_H_ */
