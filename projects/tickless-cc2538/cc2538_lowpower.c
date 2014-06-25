/*
 * Copyright 2013 OpenMote Technologies, S.L.
 */

/**
 *
 * @file       cc2538_lowpower.c
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
#define portNVIC_PENDSVSET_BIT	    ( 1UL << 28UL )

/* The frequency at which RTC will run. */
#define lpCLOCK_INPUT_FREQUENCY 	( 32768UL )

/*================================ typedef ==================================*/

/*=============================== variables =================================*/

/* Calculate how many clock increments make up a single tick period. */
static uint32_t ulAlarmValueForOneTick = ( ( lpCLOCK_INPUT_FREQUENCY / configTICK_RATE_HZ ) - 1 );

/* Holds the maximum number of ticks that can be suppressed - which is
basically how far into the future an interrupt can be generated. Set
during initialisation. */
static TickType_t xMaximumPossibleSuppressedTicks = 0;

/* Flag set from the tick interrupt to allow the sleep processing to know if
sleep mode was exited because of an RTC interrupt or a different interrupt. */
static volatile uint32_t ulTickFlag = pdFALSE;

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
    /* Ensure the 32.768 kHz oscillator is enabled. */
    /* The 32.768 kHz oscillator is enabled by default */
    
    /* Wait until the RTC is sourced and stable */
    while(HWREG(SYS_CTRL_CLOCK_STA) & SYS_CTRL_CLOCK_STA_OSC32K);
    
    /* The RTC alarm interrupt is used as the tick interrupt. Ensure the alarm status starts clear. */
    IntPendClear(INT_SMTIM);

    /* Tick interrupt MUST execute at the lowest interrupt priority. */
    IntPrioritySet(INT_SMTIM, configPERIPHERAL_LOWEST_INTERRUPT_PRIORITY);
    
    /* Ensure the RTC can bring the CPU out of sleep mode. */
    GPIOIntWakeupEnable(GPIO_IWE_SM_TIMER);

    /* See the comments where xMaximumPossibleSuppressedTicks is declared. */
    xMaximumPossibleSuppressedTicks = ULONG_MAX / ulAlarmValueForOneTick;
	
    /* Save the current RTC value to enable future calculations */
	ulPreviousCounterValue = SleepModeTimerCountGet();

    SleepModeTimerCompareSet(ulPreviousCounterValue + ulAlarmValueForOneTick);
    
    /* Enable wakeup from alarm 0 in the RTC and power manager.  */
    IntEnable(INT_SMTIM);
}


/* Override the default definition of vPortSuppressTicksAndSleep() that is weakly
defined in the FreeRTOS Cortex-M3 port layer with a version that manages the
asynchronous timer (RTC), as the tick is generated from the low power RTC and
not the SysTick as would normally be the case on a Cortex-M. */
void vPortSuppressTicksAndSleep( TickType_t xExpectedIdleTime )
{
    uint32_t ulCounterValue, ulCompleteTickPeriods;
    uint32_t ulCurrentCounterValue, ulElapsedCounterValue;
    eSleepModeStatus eSleepAction;
    TickType_t xModifiableIdleTime;
    const TickType_t xPm1IdleTime = 10;
    const TickType_t xPm2IdleTime = 100;

	/* THIS FUNCTION IS CALLED WITH THE SCHEDULER SUSPENDED. */

	/* Make sure the RTC reload value does not overflow the counter. */
	if( xExpectedIdleTime > xMaximumPossibleSuppressedTicks )
	{
		xExpectedIdleTime = xMaximumPossibleSuppressedTicks;
	}

	/* Calculate the reload value required to wait xExpectedIdleTime tick
	periods. */
	ulCounterValue = ulAlarmValueForOneTick * xExpectedIdleTime;

	/* Stop RTC momentarily.  The time RTC is stopped for is not accounted for
	in this implementation (as it is in the generic implementation) because the
	clock is so slow it is unlikely to be stopped for a complete count period
	anyway. */
	prvDisableRTC();

	/* Enter a critical section but don't use the taskENTER_CRITICAL() method as
	that will mask interrupts that should exit sleep mode. */
	__asm volatile ( "cpsid i" );
	__asm volatile ( "dsb" );
	__asm volatile ( "isb" );

	/* The tick flag is set to false before sleeping.  If it is true when sleep
	mode is exited then sleep mode was probably exited because the tick was
	suppressed for the entire xExpectedIdleTime period. */
	ulTickFlag = pdFALSE;

	/* If a context switch is pending then abandon the low power entry as
	the context switch might have been pended by an external interrupt that
	requires processing. */
	eSleepAction = eTaskConfirmSleepModeStatus();
	if( eSleepAction == eAbortSleep )
	{
		/* Restart tick. */
		prvEnableRTC();

		/* Re-enable interrupts - see comments above the cpsid instruction()
		above. */
		__asm volatile ( "cpsie i" );
	}
	else
	{
	    /* Read the (absolute) current RTC value */
	    ulCurrentCounterValue = SleepModeTimerCountGet();
	    
	    /* Compute the (relative) current RTC value with respect to the previous value */
	    ulElapsedCounterValue = ulCurrentCounterValue - ulPreviousCounterValue;
	    
	    /* Update the (absolute) previous counter RTC value with the current value */
	    ulPreviousCounterValue = ulCurrentCounterValue;

		/* Adjust the RTC value to take into account that the current time
		slice is already partially complete. */
		ulCounterValue -= ulElapsedCounterValue;

		/* Update to use the calculated overflow value. */
        SleepModeTimerCompareSet(ulCurrentCounterValue + ulCounterValue);

		/* Restart tick. */
		prvEnableRTC();

		/* Allow the application to define some pre-sleep processing.  This is
		the standard configPRE_SLEEP_PROCESSING() macro as described on the
		FreeRTOS.org website. */
		xModifiableIdleTime = xExpectedIdleTime;
		configPRE_SLEEP_PROCESSING( xModifiableIdleTime );

		/* xExpectedIdleTime being set to 0 by configPRE_SLEEP_PROCESSING()
		means the application defined code has already executed the wait/sleep
		instruction. */
		if( xModifiableIdleTime > 0 )
		{
			/* The sleep mode used is dependent on the expected idle time
			as the deeper the sleep the longer the wake up time.  See the
			comments at the top of main_low_power.c.  Note xRegulatorOffIdleTime
			is set purely for convenience of demonstration and is not intended
			to be an optimised value. */
			if( xModifiableIdleTime < xPm1IdleTime )
			{
				/* A slightly lower power sleep mode with a longer wake up
				time. */
				SysCtrlPowerModeSet(SYS_CTRL_PM_NOACTION);
				SysCtrlSleep();
			}
			else if(xModifiableIdleTime < xPm2IdleTime ) {
			    /* A slightly higher power sleep mode with a faster wake up
				time. */
				SysCtrlPowerModeSet(SYS_CTRL_PM_1);
                SysCtrlDeepSleep();
			}
			else
			{
				/* A slightly higher power sleep mode with a faster wake up
				time. */
				SysCtrlPowerModeSet(SYS_CTRL_PM_2);
                SysCtrlDeepSleep();
			}
		}

		/* Allow the application to define some post sleep processing.  This is
		the standard configPOST_SLEEP_PROCESSING() macro, as described on the
		FreeRTOS.org website. */
		configPOST_SLEEP_PROCESSING( xModifiableIdleTime );

		/* Stop RTC.  Again, the time the clock is stopped for in not accounted
		for here (as it would normally be) because the clock is so slow it is
		unlikely it will be stopped for a complete count period anyway. */
		prvDisableRTC();

		/* Re-enable interrupts - see comments above the cpsid instruction()
		above. */
		__asm volatile ( "cpsie i" );
		__asm volatile ( "dsb" );
		__asm volatile ( "isb" );

		if( ulTickFlag != pdFALSE ) // True (Sleep mode was exited because the tick was suppressed for the entire xExpectedIdleTime period)
		{
            /* Read the (absolute) current RTC value */
	        ulCurrentCounterValue = SleepModeTimerCountGet();
	        
	        /* Compute the (relative) current RTC value with respect to the previous value */
	        ulElapsedCounterValue = ulCurrentCounterValue - ulPreviousCounterValue;
	        
	        /* Update the (absolute) previous counter RTC value with the current value */
	        ulPreviousCounterValue = ulCurrentCounterValue;

			/* The tick interrupt has already executed, although because this
			function is called with the scheduler suspended the actual tick
			processing will not occur until after this function has exited.
			Reset the reload value with whatever remains of this tick period. */
			ulCounterValue = ulAlarmValueForOneTick - ulElapsedCounterValue;

			/* Use the calculated reload value. */
            SleepModeTimerCompareSet(ulCurrentCounterValue + ulCounterValue);

			/* The tick interrupt handler will already have pended the tick
			processing in the kernel.  As the pending tick will be processed as
			soon as this function exits, the tick value	maintained by the tick
			is stepped forward by one less than the	time spent sleeping.  The
			actual stepping of the tick appears later in this function. */
			ulCompleteTickPeriods = xExpectedIdleTime - 1UL;
		}
		else // False (Sleep mode was exited because the tick was suppressed for less than the entire xExpectedIdleTime period)
		{
		    /* Read the (absolute) current RTC value */
	        ulCurrentCounterValue = SleepModeTimerCountGet();
	        
	        /* Compute the (relative) current RTC value with respect to the previous value */
	        ulElapsedCounterValue = ulCurrentCounterValue - ulPreviousCounterValue;
	        
	        /* Update the (absolute) previous counter RTC value with the current value */
	        ulPreviousCounterValue = ulCurrentCounterValue;
	        
			/* Something other than the tick interrupt ended the sleep.  How
			many complete tick periods passed while the processor was
			sleeping? */
			ulCompleteTickPeriods = ( ( uint32_t ) ulElapsedCounterValue ) / ulAlarmValueForOneTick;

			/* The reload value is set to whatever fraction of a single tick
			period remains. */
			ulCounterValue = ( ( ulCounterValue ) - ( ulCompleteTickPeriods * ulAlarmValueForOneTick ) );
			if( ulCounterValue == 0 )
			{
				/* There is no fraction remaining. */
				ulCounterValue = ulAlarmValueForOneTick;
				ulCompleteTickPeriods++;
			}
			
			/* Update to use the calculated overflow value. */
            SleepModeTimerCompareSet(ulCurrentCounterValue + ulCounterValue);
		}

		/* Restart RTC so it runs up to the reload value.  The reload value
		will get set to the value required to generate exactly one tick period
		the next time the TIM2 interrupt executes. */
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

