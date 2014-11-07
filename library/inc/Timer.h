/*
 * Copyright 2013 OpenMote Technologies, S.L.
 */

/**
 *
 * @file       Timer.h
 * @author     Pere Tuset-Peiro (peretuset@openmote.com)
 * @version    v0.1
 * @date       May, 2014
 * @brief
 * @ingroup
 *
 */

#ifndef TIMER_H_
#define TIMER_H_

#include <stdint.h>

#include "Callback.h"

class Timer
{

friend class InterruptHandler;

public:
    Timer(uint32_t peripheral_, uint32_t base_, uint32_t source_, uint32_t config_, uint32_t interrupt_, uint32_t interrupt_mode_);
    uint32_t getBase(void);
    uint32_t getSource(void);
    void init(uint32_t frequency_);
    void start(void);
    void stop(void);
    void setCallback(Callback* callback_);
    void clearCallback(void);
    void enableInterrupt(void);
    void disableInterrupt(void);
protected:
    void interruptHandler(void);
private:
    uint32_t peripheral;
    uint32_t base;
    uint32_t source;
    uint32_t config;
    uint32_t interrupt;
    uint32_t interrupt_mode;
    uint32_t frequency;

    Callback* callback;
};

#endif /* TIMER_H_ */
