/*
 * Copyright 2013 OpenMote Technologies, S.L.
 */

/**
 *
 * @file       hello-world.c
 * @author     Pere Tuset-Peiro (peretuset@openmote.com)
 * @version    v0.1
 * @date       May, 2014
 * @brief
 * @ingroup
 *
 */

/*================================ include ==================================*/

#include "Board.h"
#include "Led.h"

#include "interrupt.h"
#include "sleepmode.h"
#include "sys_ctrl.h"

#include "hw_ints.h"
#include "hw_sys_ctrl.h"
#include "hw_types.h"

#include "openmote-cc2538.h"

/*================================ define ===================================*/

#define DELAY_TICKS                 ( 320000 )

/*================================ typedef ==================================*/

/*=============================== variables =================================*/

Board board;

Led led_green(LED_GREEN_PORT, LED_GREEN_PIN);
Led led_orange(LED_ORANGE_PORT, LED_ORANGE_PIN);
Led led_red(LED_RED_PORT, LED_RED_PIN);
Led led_yellow(LED_YELLOW_PORT, LED_YELLOW_PIN);

Led debug_tick(GPIO_DEBUG_AD0_PORT, GPIO_DEBUG_AD0_PIN);
Led debug_idle(GPIO_DEBUG_AD1_PORT, GPIO_DEBUG_AD1_PIN);
Led debug_task(GPIO_DEBUG_AD2_PORT, GPIO_DEBUG_AD2_PIN);

/*=============================== prototypes ================================*/

/*================================= public ==================================*/

int main(void)
{
    uint32_t ulAlarmCurrentValue;
    
    led_green.on();

    SysCtrlPowerModeSet(SYS_CTRL_PM_2);
    GPIOIntWakeupEnable(GPIO_IWE_SM_TIMER); 
    
    IntPendClear(INT_SMTIM);
    
    ulAlarmCurrentValue = SleepModeTimerCountGet();
    SleepModeTimerCompareSet(ulAlarmCurrentValue + 32768);
    
    /* Enable wakeup from alarm 0 in the RTC and power manager.  */
    IntEnable(INT_SMTIM);
    
    IntMasterEnable();
            
    while(true){
        led_red.toggle();
        debug_task.toggle();
        SysCtrlDeepSleep();
    }

    return 0;
}

/*================================ private ==================================*/

