/*
 * Copyright 2013 OpenMote Technologies, S.L.
 */

/**
 *
 * @file       Timer.cpp
 * @author     Pere Tuset-Peiro (peretuset@openmote.com)
 * @version    v0.1
 * @date       May, 2014
 * @brief
 * @ingroup
 *
 */

/*================================ include ==================================*/

#include "Timer.h"

#include "interrupt.h"
#include "sys_ctrl.h"
#include "gptimer.h"

#include "hw_memmap.h"
#include "hw_ints.h"
#include "hw_types.h"

/*================================ define ===================================*/

/*================================ typedef ==================================*/

/*=============================== variables =================================*/

/*=============================== prototypes ================================*/

/*================================= public ==================================*/

Timer::Timer(uint32_t peripheral_, uint32_t base_, uint32_t source_, uint32_t config_, uint32_t interrupt_, uint32_t interrupt_mode_):
    peripheral(peripheral_), base(base_), source(source_), config(config_), interrupt(interrupt_), interrupt_mode(interrupt_mode_)
{
}

uint32_t Timer::getBase(void)
{
    return base;
}

uint32_t Timer::getSource(void)
{
    return source;
}

void Timer::init(uint32_t frequency_)
{
    // Store the Timer frequency
    frequency = frequency_;

    // Disable peripheral previous to configuring it
    TimerDisable(base, source);

    // Enable peripheral except in deep sleep modes (e.g. LPM1, LPM2, LPM3)
    SysCtrlPeripheralEnable(peripheral);
    SysCtrlPeripheralSleepEnable(peripheral);
    SysCtrlPeripheralDeepSleepDisable(peripheral);

    // Configure the peripheral
    TimerConfigure(base, config);

    // Set the frequency
    TimerLoadSet(base, source, frequency);
}

void Timer::start(void)
{
    // Enable the timer
    TimerEnable(base, source);
}

void Timer::stop(void)
{
    // Disable the timer
    TimerDisable(base, source);
}

void Timer::setCallback(Callback* callback_)
{
    callback = callback_;
}

void Timer::clearCallback(void)
{
    callback = nullptr;
}

void Timer::interruptEnable(void)
{
    InterruptHandler::getInstance().setInterruptHandler(this);

    // Enable Timer interrupts
    TimerIntEnable(base, interrupt_mode);

    // Set the Timer interrupt priority
    // IntPrioritySet(interrupt, (7 << 5));

    // Enable the Timer interrupt
    IntEnable(interrupt);
}

void Timer::interruptDisable(void)
{
    // Diisable Timer interrupts
    TimerIntDisable(base, interrupt_mode);

    // Disable the Timer interrupt
    IntDisable(interrupt);
}

/*=============================== protected =================================*/

void Timer::interruptHandler(void)
{
    if (callback != nullptr)
    {
        callback->execute();
    }
}

/*================================ private ==================================*/
