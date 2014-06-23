/*
 * Copyright 2013 OpenMote Technologies, S.L.
 */

/**
 *
 * @file       freertos-tickless.c
 * @author     Pere Tuset-Peiro (peretuset@openmote.com)
 * @version    v0.1
 * @date       May, 2014
 * @brief
 * @ingroup
 *
 */
 
/*================================ include ==================================*/

/* FreeRTOS includes */
#include "FreeRTOS.h"
#include "task.h"

/* ST library functions */
#include "stm32l1xx.h"

/* OpenMote definitions */
#include "openmote-stm32l1.h"

/*================================ define ===================================*/

#define RED_LED_TASK_PRIORITY           ( tskIDLE_PRIORITY + 1 )

/*================================ typedef ==================================*/

/*=============================== variables =================================*/

GPIO_InitTypeDef GPIO_InitStructure;

/*=============================== prototypes ================================*/

static void prvSetupHardware( void );
static void prvRedLedTask(void *pvParameters);

/*================================= public ==================================*/

static void prvRedLedTask( void *pvParameters ) {
	while(1) {
		GPIO_WriteBit(LED_RED_PORT, LED_RED_PIN, 1);
		vTaskDelay(1000 / portTICK_RATE_MS);
        GPIO_WriteBit(LED_RED_PORT, LED_RED_PIN, 0);
        vTaskDelay(1000 / portTICK_RATE_MS);
	}
}

int main( void )
{
	/* Prepare the hardware to run this demo. */
	prvSetupHardware();
    
    xTaskCreate(prvRedLedTask, ( const char * ) "Red", 128, NULL, RED_LED_TASK_PRIORITY, NULL );
    
	/* Start the scheduler running running. */
	vTaskStartScheduler();
	
	/* This line will never be reached. */
	return 0;
}

void vAssertCalled( unsigned long ulLine, const char * const pcFileName )
{
volatile unsigned long ulSetToNonZeroInDebuggerToContinue = 0;

	/* Parameters are not used. */
	( void ) ulLine;
	( void ) pcFileName;

	taskENTER_CRITICAL();
	{
		while( ulSetToNonZeroInDebuggerToContinue == 0 )

		{
			__asm volatile( "NOP" );
			__asm volatile( "NOP" );
		}
	}
	taskEXIT_CRITICAL();
}

void vMainPostStopProcessing( void )
{
    extern void SetSysClock( void );

	/* The STOP low power mode has been exited.  Reconfigure the system clocks
	ready for normally running again. */
	SetSysClock();
}

/*================================ private ==================================*/

static void prvSetupHardware( void )
{
	/* Essential on STM32 Cortex-M devices. */
	NVIC_PriorityGroupConfig( NVIC_PriorityGroup_4 );

	/* Systick is fed from HCLK/8. */
	SysTick_CLKSourceConfig( SysTick_CLKSource_HCLK_Div8 );

	/* Set MSI clock range to ~4.194MHz. */
	RCC_MSIRangeConfig( RCC_MSIRange_6 );

	/* Enable the GPIOs clocks. */
	RCC_AHBPeriphClockCmd( RCC_AHBPeriph_GPIOA | RCC_AHBPeriph_GPIOB | RCC_AHBPeriph_GPIOC |                          RCC_AHBPeriph_GPIOD | RCC_AHBPeriph_GPIOE | RCC_AHBPeriph_GPIOH, ENABLE );

	/* Enable comparator clocks. */
	RCC_APB1PeriphClockCmd( RCC_APB1Periph_COMP, ENABLE );

	/* Enable SYSCFG clocks. */
	RCC_APB2PeriphClockCmd( RCC_APB2Periph_SYSCFG , ENABLE );

	/* Set internal voltage regulator to 1.5V. */
	PWR_VoltageScalingConfig( PWR_VoltageScaling_Range2 );

	/* Wait Until the Voltage Regulator is ready. */
	while( PWR_GetFlagStatus( PWR_FLAG_VOS ) != RESET );
	
    GPIO_InitStructure.GPIO_Pin = LED_GREEN_PIN;
    GPIO_InitStructure.GPIO_Mode = LED_GREEN_MODE;
    GPIO_InitStructure.GPIO_OType = LED_GREEN_TYPE;
    GPIO_InitStructure.GPIO_Speed = LED_GREEN_SPEED;
    GPIO_InitStructure.GPIO_PuPd = LED_GREEN_PUSH;
    GPIO_Init(LED_GREEN_PORT, &GPIO_InitStructure);
    
    GPIO_InitStructure.GPIO_Pin = LED_RED_PIN;
    GPIO_InitStructure.GPIO_Mode = LED_RED_MODE;
    GPIO_InitStructure.GPIO_OType = LED_RED_TYPE;
    GPIO_InitStructure.GPIO_Speed = LED_RED_SPEED;
    GPIO_InitStructure.GPIO_PuPd = LED_RED_PUSH;
    GPIO_Init(LED_RED_PORT, &GPIO_InitStructure);
    
    GPIO_WriteBit(LED_GREEN_PORT, LED_GREEN_PIN, 0);
}

