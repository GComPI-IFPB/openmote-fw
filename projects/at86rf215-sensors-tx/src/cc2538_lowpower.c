/**
 * @file       cc2538_lowpower.c
 * @author     Pere Tuset-Peiro (peretuset@openmote.com)
 * @version    v0.1
 * @date       November, 2018
 * @brief
 *
 * @copyright  Copyright 2015, OpenMote Technologies, S.L.
 *             This file is licensed under the GNU General Public License v2.
 */

#include "FreeRTOSConfig.h"
   
#if (configUSE_TICKLESS_IDLE != 0)
   
/*================================ include ==================================*/

#include "FreeRTOS.h"
#include "limits.h"
#include "task.h"

#include "platform_types.h"

#include "gpio.h"
#include "interrupt.h"
#include "sleepmode.h"
#include "sys_ctrl.h"

#include "hw_ints.h"
#include "hw_sys_ctrl.h"
#include "hw_types.h"

/*================================ define ===================================*/

/* Constants required to pend a PendSV interrupt from the tick ISR if the
   preemptive scheduler is being used. These are just standard bits and registers
   within the Cortex-M core itself. */
#define portNVIC_PENDSVSET_BIT	    ( 1UL << 28UL )

#define configTICK_LOWEST_INTERRUPT_PRIORITY        ( configLIBRARY_LOWEST_INTERRUPT_PRIORITY << (8 - configPRIO_BITS) )
   
/* The frequency and drift at which RTC will run. */
#define lpCLOCK_INPUT_FREQUENCY 	  ( 32768UL )
#define lpCLOCK_INPUT_DRIFT         ( 0 )

/* The minimum number of ticks in each sleep mode and the number of ticks
   to wake up from each sleep mode. */
#define LPM1_MINIMUM_IDLE_TICKS     ( 10 )
#define LPM1_WAKEUP_TICKS           ( 0 )
#define LPM2_MINIMUM_IDLE_TICKS     ( 100 )
#define LPM2_WAKEUP_TICKS           ( 9 )

/*================================ typedef ==================================*/

/*=============================== variables =================================*/

/* Calculate how many clock increments make up a single tick period */
static uint32_t ulCounterValueForOneTick = ( ( lpCLOCK_INPUT_FREQUENCY / configTICK_RATE_HZ ) );

/* Flag set from the tick interrupt to allow the sleep processing to know if
   sleep mode was exited because of an RTC interrupt or a different interrupt */
static volatile uint32_t ulTickFlag = pdFALSE;

/* Holds the maximum number of ticks that can be suppressed */
static uint32_t xMaximumPossibleSuppressedTicks;

/*=============================== prototypes ================================*/

void SleepTimer_Handler(void);

static void prvEnableRTC(void);
static void prvDisableRTC(void);

extern BoardParams board_params;

extern void board_sleep(TickType_t xModifiableIdleTime);
extern void board_wakeup(TickType_t xModifiableIdleTime);

/*================================= public ==================================*/

/* Override the default definition of vPortSetupTimerInterrupt() that is weakly
   defined in the FreeRTOS Cortex-M3 port layer with a version that configures the
   real-time clock (RTC) to generate the tick interrupt */
void vPortSetupTimerInterrupt( void )
{	
    /* If using the 32.768 kHz oscillator wait until it becomes stable */
    if (board_params.bExternalOsc32k == true)
    {
        while(HWREG(SYS_CTRL_CLOCK_STA) & SYS_CTRL_CLOCK_STA_SYNC_32K);
        while(!(HWREG(SYS_CTRL_CLOCK_STA) & SYS_CTRL_CLOCK_STA_SYNC_32K));
    }

    /* Register the SleepTimer interrupt handler */
    IntRegister(INT_SMTIM, SleepTimer_Handler);
    
    /* The RTC interrupt is used as the tick interrupt. Ensure it starts clear. */
    IntPendClear(INT_SMTIM);

    /* Tick interrupt MUST execute at the lowest interrupt priority. */
    IntPrioritySet(INT_SMTIM, configTICK_LOWEST_INTERRUPT_PRIORITY);
    
    /* Ensure the RTC can bring the CPU out of sleep mode. */
    GPIOIntWakeupEnable(GPIO_IWE_SM_TIMER);

    /* See the comments where xMaximumPossibleSuppressedTicks is declared. */
    xMaximumPossibleSuppressedTicks = ULONG_MAX / ulCounterValueForOneTick;
    
    /* Enable the RTC interrupt to wake up from sleep modes. */
    IntEnable(INT_SMTIM);
}

/* Override the default definition of vPortSuppressTicksAndSleep() that is weakly
   defined in the FreeRTOS Cortex-M3 port layer with a version that manages the
   asynchronous timer (RTC), as the tick is generated from the low power RTC and
   not the SysTick as would normally be the case on a Cortex-M */
void vPortSuppressTicksAndSleep( TickType_t xExpectedIdleTicks )
{
  static uint32_t ulPreviousTickValue;
  uint32_t ulCounterValue, ulCompleteTickPeriods;
  uint32_t ulCurrentCounterValue, ulElapsedCounterValue;
  eSleepModeStatus eSleepAction;
  TickType_t xModifiableIdleTicks;

	/* THIS FUNCTION IS CALLED WITH THE SCHEDULER SUSPENDED */

	/* Make sure the RTC reload value does not overflow the counter */
	if(xExpectedIdleTicks > xMaximumPossibleSuppressedTicks)
	{
		xExpectedIdleTicks = xMaximumPossibleSuppressedTicks;
	}

	/* Calculate the reload value required to wait xExpectedIdleTicks tick periods */
	ulCounterValue = ulCounterValueForOneTick * xExpectedIdleTicks;

	/* Enter a critical section but don't use the taskENTER_CRITICAL() method as
	   that will mask interrupts that should exit sleep mode */
	__asm volatile ( "cpsid i" );
	__asm volatile ( "dsb" );
	__asm volatile ( "isb" );

	/* The tick flag is set to false before sleeping.  If it is true when sleep
	   mode is exited then sleep mode was probably exited because the tick was
	   suppressed for the entire xExpectedIdleTicks period */
	ulTickFlag = pdFALSE;

	/* If a context switch is pending then abandon the low power entry as
	   the context switch might have been pended by an external interrupt that
	   requires processing */
	eSleepAction = eTaskConfirmSleepModeStatus();
	if(eSleepAction == eAbortSleep)
	{
		/* Re-enable interrupts */
		__asm volatile ( "cpsie i" );
	}
	else
	{
    /* Read the (absolute) current RTC value. */
    ulCurrentCounterValue = SleepModeTimerCountGet();
      
    /* Compute the (relative) current RTC value with respect to the previous value. */
    ulElapsedCounterValue = ulCurrentCounterValue - ulPreviousTickValue;
      
    /* Update the (absolute) previous counter RTC value with the current value. */
    ulPreviousTickValue = ulCurrentCounterValue;
		
		/* Use the calculated reload value. */
    SleepModeTimerCompareSet(ulCurrentCounterValue + ulCounterValue);

		/* Allow the application to define some pre-sleep processing. This is
		   the standard configPRE_SLEEP_PROCESSING() macro as described on the
		   FreeRTOS.org website. */
		configPRE_SLEEP_PROCESSING( xExpectedIdleTicks );

		/* xExpectedIdleTicks being set to 0 by configPRE_SLEEP_PROCESSING()
		   means the application defined code has already executed the wait/sleep
		   instruction. */
		if( xModifiableIdleTicks > 0 )
		{
			/* The sleep mode used is dependent on the expected idle time
			   as the deeper the sleep the longer the wake up time. See the
			   comments at the top of file. */
			if( ulCounterValue < LPM1_MINIMUM_IDLE_TICKS )
			{
        /* A high low power sleep mode with instantaneous wake up
           time (4 mA, 0 us + 0 us). */
        SysCtrlPowerModeSet(SYS_CTRL_PM_NOACTION);
			}
			else if( ulCounterValue < LPM2_MINIMUM_IDLE_TICKS )
			{
			  /* A slightly lower power sleep mode with a faster wake up
				 time (650 uA, 0.5 us + 4 us). */
			  SysCtrlPowerModeSet(SYS_CTRL_PM_1);
			}
			else
			{
			  /* A true lower power sleep mode with a longer wake up
           time (1.6 uA, 136 us + 136 us) . */
        SysCtrlPowerModeSet(SYS_CTRL_PM_2);
			}
			
      /* Go to LPMx mode and wait for the tick to wake us up */
      SysCtrlDeepSleep();
		}

		/* Allow the application to define some post sleep processing. This is
		   the standard configPOST_SLEEP_PROCESSING() macro, as described on the
		   FreeRTOS.org website. */
		configPOST_SLEEP_PROCESSING( xModifiableIdleTicks );

		/* Re-enable interrupts - see comments above the cpsid instruction()
		   above. */
		__asm volatile ( "cpsie i" );
		__asm volatile ( "dsb" );
		__asm volatile ( "isb" );

    /* Sleep mode was exited because the tick was suppressed for the
       entire xExpectedIdleTicks period */
		if(ulTickFlag != pdFALSE)
		{
      /* Read the (absolute) current RTC value */
      ulCurrentCounterValue = SleepModeTimerCountGet();
      
      /* Compute the (relative) current RTC value with respect to the previous value */
      ulElapsedCounterValue = ulCurrentCounterValue - ulPreviousTickValue;
          
      /* The tick interrupt has already executed, although because this
         function is called with the scheduler suspended the actual tick
         processing will not occur until after this function has exited.
         Reset the reload value with whatever remains of this tick period */
      ulCounterValue = ulCounterValueForOneTick - ulElapsedCounterValue;

      /* Use the calculated reload value */
      SleepModeTimerCompareSet(ulCurrentCounterValue + ulCounterValue);

      /* The tick interrupt handler will already have pended the tick
      processing in the kernel.  As the pending tick will be processed as
      soon as this function exits, the tick value	maintained by the tick
      is stepped forward by one less than the	time spent sleeping.  The
      actual stepping of the tick appears later in this function */
      ulCompleteTickPeriods = xExpectedIdleTicks - 1UL;
		}
		else /* Sleep mode was exited because the tick was suppressed for less
		        than the entire xExpectedIdleTicks period */
		{
      /* Read the (absolute) current RTC value. */
      ulCurrentCounterValue = SleepModeTimerCountGet();
        
      /* Compute the (relative) current RTC value with respect to the previous value. */
      ulElapsedCounterValue = ulCurrentCounterValue - ulPreviousTickValue;
        
      /* Something other than the tick interrupt ended the sleep.  How
       many complete tick periods passed while the processor was
       sleeping? */
      ulCompleteTickPeriods = ( ( uint32_t ) ulElapsedCounterValue ) / ulCounterValueForOneTick;

      /* The reload value is set to whatever fraction of a single tick
       period remains. */
      ulCounterValue = ( ( ulCounterValue ) - ( ulCompleteTickPeriods * ulCounterValueForOneTick ) );
      if( ulCounterValue == 0 )
      {
        /* There is no fraction remaining. */
        ulCounterValue = ulCounterValueForOneTick;
        ulCompleteTickPeriods++;
      }
						
			/* Update to use the calculated overflow value. */
      SleepModeTimerCompareSet(ulCurrentCounterValue + ulCounterValue);
		}

		/* Wind the tick forward by the number of tick periods that the CPU
		  remained in a low power state. */
		vTaskStepTick( ulCompleteTickPeriods );
	}
}

void SleepTimer_Handler(void)
{
  uint32_t ulCurrentCounterValue;
  
  /* Wait until the RTC is stable. */
  while(HWREG(SYS_CTRL_CLOCK_STA) & SYS_CTRL_CLOCK_STA_SYNC_32K);
  while((!HWREG(SYS_CTRL_CLOCK_STA)) & SYS_CTRL_CLOCK_STA_SYNC_32K);

  /* Ensure the interrupt is clear before exiting. */
  IntPendClear(INT_SMTIM);

  /* Get the current value of the RTC. */
  ulCurrentCounterValue = SleepModeTimerCountGet();

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
  portCLEAR_INTERRUPT_MASK_FROM_ISR(0);

  /* If this is the first tick since exiting tickless mode then the RTC needs
     o be reconfigured to generate interrupts at the defined tick frequency. */
  SleepModeTimerCompareSet(ulCurrentCounterValue + ulCounterValueForOneTick);

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

#endif