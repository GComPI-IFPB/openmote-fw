/*
 * Copyright 2013 OpenMote Technologies, S.L.
 */

/**
 *
 * @file       SysTick.h
 * @author     Pere Tuset-Peiro (peretuset@openmote.com)
 * @version    v0.1
 * @date       May, 2014
 * @brief
 * @ingroup
 *
 */

#ifndef SYSTICK_H_
#define SYSTICK_H_

#include <stdint.h>

#include "Callback.h"


class SysTick
{

friend class InterruptHandler;

public:
    SysTick(uint32_t period_);
    void init(void);
    void start(void);
    void stop(void);
    void setCallback(Callback* callback_);
    void clearCallback(void);
    void enableInterrupt(void);
    void disableInterrupt(void);
protected:
    void interruptHandler(void);
private:
    uint32_t period;

    Callback* callback;
};

#endif /* SYSTICK_H_ */
