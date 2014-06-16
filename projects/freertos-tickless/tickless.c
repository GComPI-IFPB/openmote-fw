/*
 * Copyright 2013 OpenMote Technologies, S.L.
 */

/**
 *
 * @file       tickless.c
 * @author     Pere Tuset-Peiro (peretuset@openmote.com)
 * @version    v0.1
 * @date       May, 2014
 * @brief
 * @ingroup
 *
 */

/*================================ include ==================================*/

#include "limits.h"

#include "FreeRTOS.h"
#include "task.h"

#include "gpio.h"
#include "interrupt.h"
#include "sleepmode.h"
#include "sys_ctrl.h"

#include "hw_ints.h"
#include "hw_sys_ctrl.h"
#include "hw_types.h"

/*================================ define ===================================*/

/* Constants required to pend a PendSV interrupt from the tick ISR if the
preemptive scheduler is being used.  These are just standard bits and registers
within the Cortex-M core itself. */
#define portNVIC_PENDSVSET_BIT	( 1UL << 28UL )

/*================================ typedef ==================================*/

/*=============================== variables =================================*/

/* Calculate how many clock increments make up a single tick period. */
static uint32_t ulAlarmValueForOneTick = ( configSYSTICK_CLOCK_HZ / configTICK_RATE_HZ ) - 1;

/* Holds the maximum number of ticks that can be suppressed - which is
basically how far into the future an interrupt can be generated. Set
during initialisation. */
static TickType_t xMaximumPossibleSuppressedTicks = 0;

/* Flag set from the tick interrupt to allow the sleep processing to know if
sleep mode was exited because of an RTC interrupt or a different interrupt. */
static volatile uint32_t ulTickFlag = pdFALSE;

/* The RTC counter is stopped temporarily each time it is re-programmed.  The
following variable offsets the RTC counter alarm value by the number of RTC
counts that would typically be missed while the counter was stopped to compensate
for the lost time.  _RB_ Value needs calculating correctly. */

static uint32_t ulStoppedTimerCompensation = 2 / ( configCPU_CLOCK_HZ / configSYSTICK_CLOCK_HZ );

/* Holds the value of the RTC counter in the previous interrupt to be able to
calculate the increment. The CC2538 uses an absolute RTC counter whereas FreeRTOS
requires to know the time elapsed since the last interrupt to operate. */
static uint32_t ulPreviousCounterValue = 0;

/*=============================== prototypes ================================*/

static void prvEnableRTC(void);
static void prvDisableRTC(void);

void SleepTimerHandler(void);

/*================================= public ==================================*/

/* Override the default definition of vPortSetupTimerInterrupt() that is weakly
defined in the FreeRTOS Cortex-M3 port layer with a version that configures the
real-time clock (RTC) to generate the tick interrupt. */
void vPortSetupTimerInterrupt( void )
{
    uint32_t ulAlarmCurrentValue;
	
    /* Ensure the 32.768 kHz oscillator is enabled. */
    /* The 32.768 kHz oscillator is enabled by default */
    
    /* Ensure the RTC can bring the CPU out of sleep mode. */
    GPIOIntWakeupEnable(GPIO_IWE_SM_TIMER);

    /* Select the mode in Deep Sleep */ 
    SysCtrlPowerModeSet(SYS_CTRL_PM_2);
    
    /* The RTC alarm interrupt is used as the tick interrupt. Ensure the alarm status starts clear. */
    IntPendClear(INT_SMTIM);
		
    /* Tick interrupt MUST execute at the lowest interrupt priority. */
    IntPrioritySet(INT_SMTIM, configLIBRARY_LOWEST_INTERRUPT_PRIORITY); 

    /* See the comments where xMaximumPossibleSuppressedTicks is declared. */
    xMaximumPossibleSuppressedTicks = ULONG_MAX / ulAlarmValueForOneTick;
	
    /* Save the current RTC value to enable future calculations */
	ulPreviousCounterValue = SleepModeTimerCountGet();

	/* Configure the first RTC interrupt */
    ulAlarmCurrentValue = ulPreviousCounterValue;
    SleepModeTimerCompareSet(ulAlarmCurrentValue + ulAlarmValueForOneTick);
    
    /* Enable wakeup from alarm 0 in the RTC and power manager.  */
    IntEnable(INT_SMTIM);
}


/* Override the default definition of vPortSuppressTicksAndSleep() that is weakly
defined in the FreeRTOS Cortex-M3 port layer with a version that manages the
asynchronous timer (RTC), as the tick is generated from the low power RTC and
not the SysTick as would normally be the case on a Cortex-M. */
void vPortSuppressTicksAndSleep( TickType_t xExpectedIdleTime )
{
    uint32_t ulAlarmValue, ulCompleteTickPeriods;
    bool ulInterruptStatus;
    eSleepModeStatus eSleepAction;
    TickType_t xModifiableIdleTime;

    /* THIS FUNCTION IS CALLED WITH THE SCHEDULER SUSPENDED. */

    /* Make sure the RTC reload value does not overflow the counter. */
    if ( xExpectedIdleTime > xMaximumPossibleSuppressedTicks ) {
        xExpectedIdleTime = xMaximumPossibleSuppressedTicks;
    }

    /* Calculate the reload value required to wait xExpectedIdleTime tick periods. */
    ulAlarmValue = ulAlarmValueForOneTick * xExpectedIdleTime;
    
    if ( ulAlarmValue > ulStoppedTimerCompensation ) {
        /* Compensate for the fact that the RTC is going to be stopped momentarily. */
        ulAlarmValue -= ulStoppedTimerCompensation;
    }

    /* Stop the RTC momentarily.  The time the RTC is stopped for is accounted
    for as best it can be, but using the tickless mode will inevitably result in
    some tiny drift of the time maintained by the kernel with respect to
    calendar time. */
    prvDisableRTC();

    /* Enter a critical section but don't use the taskENTER_CRITICAL() method as
    that will mask interrupts that should exit sleep mode. */
    ulInterruptStatus = IntMasterDisable();

    /* The tick flag is set to false before sleeping.  If it is true when sleep
    mode is exited then sleep mode was probably exited because the tick was
    suppressed for the entire xExpectedIdleTime period. */
    ulTickFlag = pdFALSE;

    /* If a context switch is pending then abandon the low power entry as
    the context switch might have been pended by an external interrupt that
    requires processing. */
    eSleepAction = eTaskConfirmSleepModeStatus();
    if( eSleepAction == eAbortSleep ) {
        /* Restart the RTC. */
        prvEnableRTC();
    
        /* Re-enable interrupts - see comments above the cpsid instruction() above. */
        if ( !ulInterruptStatus ) {
            IntMasterEnable();
        }
    } else {
        /* Get the increase in number of ticks since the last time */
        ulPreviousCounterValue = SleepModeTimerCountGet() - ulPreviousCounterValue;
    
        /* Adjust the alarm value to take into account that the current time slice is already partially complete. */
        ulAlarmValue -= ulPreviousCounterValue;
        
        /* Update the current RTC value to enable future calculations */
        ulPreviousCounterValue = SleepModeTimerCountGet();
        
        /* Set the RTC interrupt in the future */
        SleepModeTimerCompareSet( ulPreviousCounterValue + ulAlarmValue );

        /* Restart the RTC. */
        prvEnableRTC();

        /* Allow the application to define some pre-sleep processing. */
        xModifiableIdleTime = xExpectedIdleTime;
        configPRE_SLEEP_PROCESSING( xModifiableIdleTime );

        /* xExpectedIdleTime being set to 0 by configPRE_SLEEP_PROCESSING()
        means the application defined code has already executed the WAIT
        instruction. */
        if( xModifiableIdleTime > 0 )
        {
            /* Find the deepest allowable sleep mode. */
            /* Sleep until something happens. */
            SysCtrlDeepSleep();
        }

        /* Allow the application to define some post sleep processing. */
        configPOST_SLEEP_PROCESSING( xModifiableIdleTime );

        /* Stop RTC.  Again, the time the SysTick is stopped for is	accounted
        for as best it can be, but using the tickless mode will	inevitably
        result in some tiny drift of the time maintained by the	kernel with
        respect to calendar time. */
        prvDisableRTC();

        /* Re-enable interrupts - see comments above the cpsid instruction() above. */
        if (!ulInterruptStatus) {
            IntMasterEnable();
        }

        if ( ulTickFlag != pdFALSE ) {
            /* Get the increase in number of ticks since the last time */
            ulPreviousCounterValue = SleepModeTimerCountGet() - ulPreviousCounterValue;
        
            /* The tick interrupt has already executed, although because this
            function is called with the scheduler suspended the actual tick
            processing will not occur until after this function has exited.
            Reset the alarm value with whatever remains of this tick period. */
            ulAlarmValue = ulAlarmValueForOneTick - ulPreviousCounterValue;
            
            /* Update the current RTC value to enable future calculations */
            ulPreviousCounterValue = SleepModeTimerCountGet();
            
            /* Set the  RTC interrupt in the future */
            SleepModeTimerCompareSet( ulPreviousCounterValue + ulAlarmValue );

            /* The tick interrupt handler will already have pended the tick
            processing in the kernel.  As the pending tick will be processed as
            soon as this function exits, the tick value	maintained by the tick
            is stepped forward by one less than the	time spent sleeping.  The
            actual stepping of the tick appears later in this function. */
            ulCompleteTickPeriods = xExpectedIdleTime - 1UL;
        } else {
            // Get the increase in number of ticks since the last time
            ulPreviousCounterValue = SleepModeTimerCountGet() - ulPreviousCounterValue;
        
            /* Something other than the tick interrupt ended the sleep.  How
            many complete tick periods passed while the processor was
            sleeping? */
            ulCompleteTickPeriods = ulPreviousCounterValue / ulAlarmValueForOneTick;

            /* The alarm value is set to whatever fraction of a single tick
            period remains. */
            ulAlarmValue = ulPreviousCounterValue - ( ulCompleteTickPeriods * ulAlarmValueForOneTick );
            
            if ( ulAlarmValue == 0 ) {
                /* There is no fraction remaining. */
                ulAlarmValue = ulAlarmValueForOneTick;
                ulCompleteTickPeriods++;
            }
            
            /* Update the current RTC value to enable future calculations */
            ulPreviousCounterValue = SleepModeTimerCountGet();
            
            /* Set the  RTC interrupt in the future */
            SleepModeTimerCompareSet( ulPreviousCounterValue + ulAlarmValue );
        }

        /* Restart the RTC so it runs up to the alarm value.  The alarm value
        will get set to the value required to generate exactly one tick period
        the next time the RTC interrupt executes. */
        prvEnableRTC();

        /* Wind the tick forward by the number of tick periods that the CPU
        remained in a low power state. */
        vTaskStepTick( ulCompleteTickPeriods );
    }
}

void SleepTimerHandler(void)
{
    uint32_t ulAlarmCurrentValue;
    
    /* Wait until the RTC is stable */
    while(HWREG(SYS_CTRL_CLOCK_STA) & SYS_CTRL_CLOCK_STA_SYNC_32K);
    while((!HWREG(SYS_CTRL_CLOCK_STA)) & SYS_CTRL_CLOCK_STA_SYNC_32K);

    /* Ensure the interrupt is clear before exiting. */
    IntPendClear(INT_SMTIM);

    /* Get the current value of the RTC */
    ulAlarmCurrentValue = SleepModeTimerCountGet();

    /* Protect incrementing the tick with an interrupt safe critical section. */
    ( void ) portSET_INTERRUPT_MASK_FROM_ISR();
    {
        if ( xTaskIncrementTick() != pdFALSE ) {
            portNVIC_INT_CTRL_REG = portNVIC_PENDSVSET_BIT;
        }

        /* Just completely clear the interrupt mask on exit by passing 0 because
        it is known that this interrupt will only ever execute with the lowest
        possible interrupt priority. */
    }
    portCLEAR_INTERRUPT_MASK_FROM_ISR( 0 );

    /* If this is the first tick since exiting tickless mode then the RTC needs
    to be reconfigured to generate interrupts at the defined tick frequency. */
    SleepModeTimerCompareSet(ulAlarmCurrentValue + ulAlarmValueForOneTick);

    /* The CPU woke because of a tick. */
    ulTickFlag = pdTRUE;
}

/*================================ private ==================================*/

static void prvEnableRTC( void )
{
    IntEnable(INT_SMTIM);
}

static void prvDisableRTC( void )
{
    IntDisable(INT_SMTIM);
}

