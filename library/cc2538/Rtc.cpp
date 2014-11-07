/*
 * Copyright 2013 OpenMote Technologies, S.L.
 */

/**
 *
 * @file       Rtc.cpp
 * @author     Pere Tuset-Peiro (peretuset@openmote.com)
 * @version    v0.1
 * @date       May, 2014
 * @brief
 * @ingroup
 *
 */

/*================================ include ==================================*/

#include "Rtc.h"
#include "InterruptHandler.h"

#include "cc2538_include.h"

/*================================ define ===================================*/

/*================================ typedef ==================================*/

/*=============================== variables =================================*/

/*=============================== prototypes ================================*/

/*================================= public ==================================*/

Rtc::Rtc(uint32_t interrupt_):
    interrupt(interrupt_)
{

}

void Rtc::init(void)
{
}

void Rtc::start(uint32_t time)
{
    uint32_t t = SleepModeTimerCountGet();
    SleepModeTimerCompareSet(t + time);
}

void Rtc::stop(void)
{
}

uint32_t Rtc::read(void)
{
    return SleepModeTimerCountGet();
}

void Rtc::setCallback(Callback* callback_)
{
    callback = callback_;
}

void Rtc::clearCallack(void)
{
    callback = nullptr;
}

void Rtc::enableInterrupt(void)
{
    InterruptHandler::getInstance().setInterruptHandler(this);

    IntEnable(interrupt);
}

void Rtc::disableInterrupt(void)
{
    IntDisable(interrupt);

    InterruptHandler::getInstance().clearInterruptHandler(this);
}

/*=============================== protected =================================*/

/*================================ private ==================================*/

void Rtc::interruptHandler(void)
{
    if (callback != nullptr)
    {
        callback->execute();
    }
}
