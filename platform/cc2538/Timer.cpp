/**
 * @file       Timer.cpp
 * @author     Pere Tuset-Peiro (peretuset@openmote.com)
 * @version    v0.1
 * @date       May, 2015
 * @brief
 *
 * @copyright  Copyright 2015, OpenMote Technologies, S.L.
 *             This file is licensed under the GNU General Public License v2.
 */

/*================================ include ==================================*/

#include "Timer.h"
#include "InterruptHandler.h"

#include "cc2538_include.h"
#include "platform_types.h"

/*================================ define ===================================*/

/*================================ typedef ==================================*/

/*=============================== variables =================================*/

/*=============================== prototypes ================================*/

/*================================= public ==================================*/

Timer::Timer(TimerConfig& config):
    config_(config)    
{
}

void Timer::init(uint32_t frequency)
{
    // Disable peripheral previous to configuring it
    TimerDisable(config_.base, config_.source);

    // Enable peripheral except in deep sleep modes (e.g. LPM1, LPM2, LPM3)
    SysCtrlPeripheralEnable(config_.peripheral);
    SysCtrlPeripheralSleepEnable(config_.peripheral);
    SysCtrlPeripheralDeepSleepDisable(config_.peripheral);

    // Configure the peripheral
    TimerConfigure(config_.base, config_.config);

    // Set the frequency
    TimerLoadSet(config_.base, config_.source, frequency);
}

void Timer::start(void)
{
    TimerEnable(config_.base, config_.source);
}

void Timer::stop(void)
{
    TimerDisable(config_.base, config_.source);
}

uint32_t Timer::read(void)
{
    return TimerValueGet(config_.base, config_.source);
}

void Timer::setCallback(Callback* callback)
{
    callback_ = callback;
}

void Timer::clearCallback(void)
{
    callback_ = nullptr;
}

void Timer::enableInterrupts(void)
{
    InterruptHandler::getInstance().setInterruptHandler(this);

    // Enable Timer interrupts
    TimerIntEnable(config_.base, config_.interrupt_mode);

    // Set the Timer interrupt priority
    // IntPrioritySet(interrupt_, (7 << 5));

    // Enable the Timer interrupt
    IntEnable(config_.interrupt);
}

void Timer::disableInterrupts(void)
{
    // Diisable Timer interrupts
    TimerIntDisable(config_.base, config_.interrupt_mode);

    // Disable the Timer interrupt
    IntDisable(config_.interrupt);
}

/*=============================== protected =================================*/

TimerConfig& Timer::getConfig(void)
{
    return config_;
}

void Timer::interruptHandler(void)
{
    if (callback_ != nullptr)
    {
        callback_->execute();
    }
}

/*================================ private ==================================*/
