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

class Timer
{

friend class InterruptHandler;

public:
    Timer(uint32_t base_, uint32_t config_);
private:
    uint32_t base;
    uint32_t config;
};

#endif /* TIMER_H_ */
