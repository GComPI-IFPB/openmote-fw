/*
 * Copyright 2013 OpenMote Technologies, S.L.
 */

/**
 *
 * @file       SleepTimer.h
 * @author     Pere Tuset-Peiro (peretuset@openmote.com)
 * @version    v0.1
 * @date       May, 2014
 * @brief
 * @ingroup
 *
 */

#ifndef SLEEP_TIMER_H_
#define SLEEP_TIMER_H_

#include <stdint.h>

#include "Callback.h"

class SleepTimer
{

friend class InterruptHandler;

public:
    SleepTimer(uint32_t interrupt);
    void init(void);
    void start(uint32_t counts);
    void stop(void);
    uint32_t read(void);
    void setCallback(Callback* callback);
    void clearCallback(void);
    void enableInterrupts(void);
    void disableInterrupts(void);
protected:
    void interruptHandler(void);
private:
    uint32_t interrupt_;

    Callback* callback_;
};

#endif /* SLEEP_TIMER_H_ */
