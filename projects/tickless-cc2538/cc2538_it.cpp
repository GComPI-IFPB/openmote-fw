/*
 * Copyright 2013 OpenMote Technologies, S.L.
 */

/**
 *
 * @file       cc2538_it.c
 * @author     Pere Tuset-Peiro (peretuset@openmote.com)
 * @version    v0.1
 * @date       May, 2014
 * @brief
 * @ingroup
 *
 */

/*================================ include ==================================*/

#include "cc2538_it.h"

#include "Led.h"

#include "interrupt.h"
#include "sleepmode.h"
#include "sys_ctrl.h"

#include "hw_ints.h"
#include "hw_sys_ctrl.h"
#include "hw_types.h"

/*================================ define ===================================*/

/*================================ typedef ==================================*/

/*=============================== variables =================================*/

extern Led debug_tick;

/*=============================== prototypes ================================*/

/*================================= public ==================================*/

void SleepTimerHandler(void)
{
    uint32_t ulAlarmCurrentValue;

    while(HWREG(SYS_CTRL_CLOCK_STA) & SYS_CTRL_CLOCK_STA_SYNC_32K);
    while(!HWREG(SYS_CTRL_CLOCK_STA) & SYS_CTRL_CLOCK_STA_SYNC_32K);

    IntPendClear(INT_SMTIM);

    debug_tick.toggle();

    ulAlarmCurrentValue = SleepModeTimerCountGet();

    SleepModeTimerCompareSet(ulAlarmCurrentValue + 32768);
}

/*================================ private ==================================*/

