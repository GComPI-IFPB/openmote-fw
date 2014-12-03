/*
 * Copyright 2013 OpenMote Technologies, S.L.
 */

/**
 *
 * @file       RadioTimer.cpp
 * @author     Pere Tuset-Peiro (peretuset@openmote.com)
 * @version    v0.1
 * @date       May, 2014
 * @brief
 * @ingroup
 *
 */

/*================================ include ==================================*/

#include "RadioTimer.h"
#include "InterruptHandler.h"

#include "cc2538_include.h"

/*================================ define ===================================*/

/*================================ typedef ==================================*/

/*=============================== variables =================================*/

/*=============================== prototypes ================================*/

/*================================= public ==================================*/

RadioTimer::RadioTimer(uint32_t interrupt):
    interrupt_(interrupt)
{
}

void RadioTimer::enable(void)
{

}

void RadioTimer::start(void)
{
    // Start the RadioTimer
    HWREG(RFCORE_SFR_MTCTRL) |= (RFCORE_SFR_MTCTRL_RUN | RFCORE_SFR_MTCTRL_SYNC);

    // Wait until clock is stable
    while(!( HWREG(RFCORE_SFR_MTCTRL) & RFCORE_SFR_MTCTRL_STATE));
}

void RadioTimer::stop(void)
{
    // Stop the RadioTimer
    HWREG(RFCORE_SFR_MTCTRL) = 0;
}

uint32_t RadioTimer::getCounter(void)
{
    uint32_t scratch;

    HWREG(RFCORE_SFR_MTMSEL) = (0x00 << RFCORE_SFR_MTMSEL_MTMSEL_S) & RFCORE_SFR_MTMSEL_MTMSEL_M;

    scratch = HWREG(RFCORE_SFR_MTM0);

    return scratch;
}

void RadioTimer::setCounter(uint32_t counter)
{

}

uint32_t RadioTimer::getPeriod(void)
{
    return 0;
}

void RadioTimer::setPeriod(uint32_t period)
{
}

uint32_t RadioTimer::getCapture(void)
{
    return 0;
}

void RadioTimer::setCapture(uint32_t capture)
{
}

void RadioTimer::setCaptureCallback(Callback* capture)
{
    capture_ = capture;
}

void RadioTimer::setCompareCallback(Callback* compare)
{
    compare_ = compare;
}

void RadioTimer::setOverflowCallback(Callback* overflow)
{
    overflow_ = overflow;
}

void RadioTimer::enableInterrupts(void)
{
    InterruptHandler::getInstance().setInterruptHandler(this);
    IntEnable(interrupt_);
}

void RadioTimer::disableInterrupts(void)
{
    IntDisable(interrupt_);

    InterruptHandler::getInstance().clearInterruptHandler(this);
}

/*=============================== protected =================================*/

void RadioTimer::interruptHandler(void)
{
    uint32_t t2irqm;
    uint32_t t2irqf;

    // Read
    t2irqm = HWREG(RFCORE_SFR_MTIRQM);
    t2irqf = HWREG(RFCORE_SFR_MTIRQF);

    // Clear pending interrupt
    IntPendClear(INT_MACTIMR);

    // Compare 1
    if ((t2irqf & RFCORE_SFR_MTIRQM_MACTIMER_OVF_COMPARE1M)& t2irqm){
        // Clear interrupt
        HWREG(RFCORE_SFR_MTIRQF)=~RFCORE_SFR_MTIRQM_MACTIMER_OVF_COMPARE1M;

        if (compare_ != nullptr) {
            compare_->execute();
        }
    }

    // Timer overflows
    else if ((t2irqf & RFCORE_SFR_MTIRQM_MACTIMER_OVF_PERM) & t2irqm){
        // Clear interrupt
        HWREG(RFCORE_SFR_MTIRQF) = ~RFCORE_SFR_MTIRQM_MACTIMER_OVF_PERM;

        if (overflow_ != nullptr) {
            overflow_->execute();
        }
    }
}

/*================================ private ==================================*/
