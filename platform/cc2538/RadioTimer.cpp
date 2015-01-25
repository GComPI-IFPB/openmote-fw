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

#define RADIOTIMER_32MHZ_TO_32KHZ_TICKS     ( 976 )

#define MTMSEL                              ( 0x07 )
#define MTMSEL_TIMER                        (( 0x00 << RFCORE_SFR_MTMSEL_MTMSEL_S ) & MTMSEL ) //*
#define MTMSEL_CAPTURE                      (( 0x01 << RFCORE_SFR_MTMSEL_MTMSEL_S ) & MTMSEL )
#define MTMSEL_PERIOD                       (( 0x02 << RFCORE_SFR_MTMSEL_MTMSEL_S ) & MTMSEL ) //*
#define MTMSEL_COMPARE1                     (( 0x03 << RFCORE_SFR_MTMSEL_MTMSEL_S ) & MTMSEL )
#define MTMSEL_COMPARE2                     (( 0x04 << RFCORE_SFR_MTMSEL_MTMSEL_S ) & MTMSEL )

#define MTMOVFSEL                           ( 0x70 )
#define MTMOVFSEL_TIMER                     (( 0x00 << RFCORE_SFR_MTMSEL_MTMOVFSEL_S ) & MTMOVFSEL ) //*
#define MTMOVFSEL_CAPTURE                   (( 0x01 << RFCORE_SFR_MTMSEL_MTMOVFSEL_S ) & MTMOVFSEL )
#define MTMOVFSEL_PERIOD                    (( 0x02 << RFCORE_SFR_MTMSEL_MTMOVFSEL_S ) & MTMOVFSEL ) //*
#define MTMOVFSEL_COMPARE1                  (( 0x03 << RFCORE_SFR_MTMSEL_MTMOVFSEL_S ) & MTMOVFSEL ) //*
#define MTMOVFSEL_COMPARE2                  (( 0x04 << RFCORE_SFR_MTMSEL_MTMOVFSEL_S ) & MTMOVFSEL )

/*================================ typedef ==================================*/

/*=============================== variables =================================*/

/*=============================== prototypes ================================*/

/*================================= public ==================================*/

RadioTimer::RadioTimer(uint32_t interrupt):
    interrupt_(interrupt)
{
}

void RadioTimer::start(void)
{
    // Set timer period to 976 ticks to have a 32786 kHz clock from a 32 MHz source
    HWREG(RFCORE_SFR_MTMSEL) = MTMSEL_PERIOD;
    HWREG(RFCORE_SFR_MTM0)   = ((RADIOTIMER_32MHZ_TO_32KHZ_TICKS >> 0) << RFCORE_SFR_MTM0_MTM0_S) & RFCORE_SFR_MTM0_MTM0_M;
    HWREG(RFCORE_SFR_MTM1)   = ((RADIOTIMER_32MHZ_TO_32KHZ_TICKS >> 8) << RFCORE_SFR_MTM1_MTM1_S) & RFCORE_SFR_MTM1_MTM1_M;

    // Set timer counter to 0 ticks
    HWREG(RFCORE_SFR_MTMSEL) = MTMSEL_TIMER;
    HWREG(RFCORE_SFR_MTM0)   = (0x00 << RFCORE_SFR_MTM0_MTM0_S) & RFCORE_SFR_MTM0_MTM0_M;
    HWREG(RFCORE_SFR_MTM1)   = (0x00 << RFCORE_SFR_MTM1_MTM1_S) & RFCORE_SFR_MTM1_MTM1_M;

    // Clear the interrupt flags
    HWREG(RFCORE_SFR_MTIRQF) = 0x00;

    // Start the timer, no 32 kHz synchronization and no latch
    HWREG(RFCORE_SFR_MTCTRL) = (RFCORE_SFR_MTCTRL_RUN);

    // Wait until the timer is stable
    while(!(HWREG(RFCORE_SFR_MTCTRL) & RFCORE_SFR_MTCTRL_STATE));
}

void RadioTimer::stop(void)
{
    // Stop the timer
    HWREG(RFCORE_SFR_MTCTRL) &= ~RFCORE_SFR_MTCTRL_RUN;
}

uint32_t RadioTimer::sleep(void)
{
    return 0;
}

void RadioTimer::wakeup(uint32_t ticks)
{
}

uint32_t RadioTimer::getCounter(void)
{
    uint32_t counter;

    // Select period register in the selector so it can be read
    HWREG(RFCORE_SFR_MTMSEL) = MTMOVFSEL_TIMER;

    // Read counter register by register
    counter  = (HWREG(RFCORE_SFR_MTMOVF0) << 0);
    counter += (HWREG(RFCORE_SFR_MTMOVF1) << 8);
    counter += (HWREG(RFCORE_SFR_MTMOVF2) << 16);

    return counter;
}

void RadioTimer::setCounter(uint32_t counter)
{
    // Select overflow timer counter
    HWREG(RFCORE_SFR_MTMSEL)  = MTMOVFSEL_TIMER;

    // Write overflow timer counter to 0 ticks
    HWREG(RFCORE_SFR_MTMOVF0) = ((counter >> 0)  << RFCORE_SFR_MTMOVF0_MTMOVF0_S) & RFCORE_SFR_MTMOVF0_MTMOVF0_M;
    HWREG(RFCORE_SFR_MTMOVF1) = ((counter >> 8)  << RFCORE_SFR_MTMOVF1_MTMOVF1_S) & RFCORE_SFR_MTMOVF1_MTMOVF1_M;
    HWREG(RFCORE_SFR_MTMOVF2) = ((counter >> 16) << RFCORE_SFR_MTMOVF2_MTMOVF2_S) & RFCORE_SFR_MTMOVF2_MTMOVF2_M;
}

uint32_t RadioTimer::getPeriod(void)
{
    uint32_t period;

    // Select overflow period register
    HWREG(RFCORE_SFR_MTMSEL) = MTMOVFSEL_PERIOD;

    // Read overflow period register
    period  = (HWREG(RFCORE_SFR_MTMOVF0) << 0);
    period += (HWREG(RFCORE_SFR_MTMOVF1) << 8);
    period += (HWREG(RFCORE_SFR_MTMOVF2) << 16);

    return period;
}

void RadioTimer::setPeriod(uint32_t period)
{
    // Reset the overflow timer counter
    setCounter(0);

    // Select overflow timer period
    HWREG(RFCORE_SFR_MTMSEL)  = MTMOVFSEL_PERIOD;

    // Write overflow timer period
    HWREG(RFCORE_SFR_MTMOVF0) = ((period >> 0)  << RFCORE_SFR_MTMOVF0_MTMOVF0_S) & RFCORE_SFR_MTMOVF0_MTMOVF0_M;
    HWREG(RFCORE_SFR_MTMOVF1) = ((period >> 8)  << RFCORE_SFR_MTMOVF1_MTMOVF1_S) & RFCORE_SFR_MTMOVF1_MTMOVF1_M;
    HWREG(RFCORE_SFR_MTMOVF2) = ((period >> 16) << RFCORE_SFR_MTMOVF2_MTMOVF2_S) & RFCORE_SFR_MTMOVF2_MTMOVF2_M;

    // Enable the overflow interrupt
    HWREG(RFCORE_SFR_MTIRQM) = RFCORE_SFR_MTIRQM_MACTIMER_OVF_PERM;
}

uint32_t RadioTimer::getCompare(void)
{
    uint32_t compare;

    // Select overflow comapre register
    HWREG(RFCORE_SFR_MTMSEL) = MTMOVFSEL_COMPARE1;

    // Read overflow compare register
    compare  = (HWREG(RFCORE_SFR_MTMOVF0) << 0);
    compare += (HWREG(RFCORE_SFR_MTMOVF1) << 8);
    compare += (HWREG(RFCORE_SFR_MTMOVF2) << 16);

    return compare;
}

void RadioTimer::setCompare(uint32_t compare)
{
    // Select overflow compare register
    HWREG(RFCORE_SFR_MTMSEL) = MTMOVFSEL_COMPARE1;

    // Write overflow compare register
    HWREG(RFCORE_SFR_MTMOVF0) = (compare << 0) & 0xFF;
    HWREG(RFCORE_SFR_MTMOVF1) = (compare << 8) & 0xFF;
    HWREG(RFCORE_SFR_MTMOVF2) = (compare << 16) & 0xFF;
}

void RadioTimer::setPeriodCallback(Callback* period)
{
    period_ = period;
}

void RadioTimer::clearPeriodCallback(void)
{
    period_ = nullptr;
}

void RadioTimer::setCompareCallback(Callback* compare)
{
    compare_ = compare;
}

void RadioTimer::clearCompareCallback(void)
{
    compare_ = nullptr;
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

    // Read interrupt registers
    t2irqm = HWREG(RFCORE_SFR_MTIRQM);
    t2irqf = HWREG(RFCORE_SFR_MTIRQF);

    // Clear pending interrupt
    IntPendClear(INT_MACTIMR);

    // Timer Compare 1 interrupt
    if ((t2irqf & RFCORE_SFR_MTIRQM_MACTIMER_OVF_COMPARE1M) & t2irqm)
    {
        // Clear interrupt
        HWREG(RFCORE_SFR_MTIRQF) &= ~RFCORE_SFR_MTIRQM_MACTIMER_OVF_COMPARE1M;

        // Execute interrupt
        if (compare_ != nullptr) compare_->execute();
    }

    // Timer overflow interrupt
    else if ((t2irqf & RFCORE_SFR_MTIRQM_MACTIMER_OVF_PERM) & t2irqm)
    {
        // Clear interrupt
        HWREG(RFCORE_SFR_MTIRQF) &= ~RFCORE_SFR_MTIRQM_MACTIMER_OVF_PERM;

        // Execute interrupt
        if (period_ != nullptr) period_->execute();
    }
}

/*================================ private ==================================*/
