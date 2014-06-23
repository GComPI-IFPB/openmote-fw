/*
 * Copyright 2013 OpenMote Technologies, S.L.
 */

/**
 *
 * @file       startup_stm32l1xx.c
 * @author     Pere Tuset-Peiro (peretuset@openmote.com)
 * @version    v0.1
 * @date       May, 2014
 * @brief
 * @ingroup
 *
 */

/*================================ include ==================================*/

#include <stdint.h>

#include "system_stm32l1xx.h"

/*================================ define ===================================*/

/*================================ typedef ==================================*/

/*=============================== prototypes ================================*/

extern int main (void);
extern void __libc_init_array(void);

void Reset_Handler(void);
void NMI_Handler(void);
void HardFault_Handler(void);
void Default_Handler(void);

extern void vPortSVCHandler(void);
extern void xPortPendSVHandler(void);
extern void xPortSysTickHandler(void);

/*=============================== variables =================================*/

static uint32_t pui32Stack[128];

extern uint32_t _etext;
extern uint32_t _data;
extern uint32_t _edata;
extern uint32_t _bss;
extern uint32_t _ebss;

__attribute__ ((section(".vectors"), used))
void (* const gVectors[])(void) =
{
   (void (*)(void))((uint32_t)pui32Stack + sizeof(pui32Stack)), // Stack pointer
   Reset_Handler,                       // Reset_Handler
   NMI_Handler,                         // NMI_Handler
   HardFault_Handler,                   // HardFault_Handler
   Default_Handler,                     // 4 MemManage_Handler
   Default_Handler,                     // 5 BusFault_Handler
   Default_Handler,                     // 6 UsageFault_Handler
   0,                                   // 7
   0,                                   // 8
   0,                                   // 9
   0,                                   // 10
   vPortSVCHandler,                         // 11 SVC_Handler
   Default_Handler,                     // 12 DebugMon_Handler
   0,                                   // 13
   xPortPendSVHandler,                  // 14 PendSV_Handler
   xPortSysTickHandler,                 // 15 SysTick_Handler
   Default_Handler,                     // 16 WWDG_IRQHandler
   Default_Handler,                     // 17 PVD_IRQHandler
   Default_Handler,                     // 18 TAMPER_STAMP_IRQHandler
   Default_Handler,                     // 19 RTC_WKUP_IRQHandler
   Default_Handler,                     // 20 FLASH_IRQHandler
   Default_Handler,                     // 21 RCC_IRQHandler
   Default_Handler,                     // 22 EXTI0_IRQHandler
   Default_Handler,                     // 23 EXTI1_IRQHandler
   Default_Handler,                     // 24 EXTI2_IRQHandler
   Default_Handler,                     // 25 EXTI3_IRQHandler
   Default_Handler,                     // 26 EXTI4_IRQHandler
   Default_Handler,                     // 27 DMA1_Channel1_IRQHandler
   Default_Handler,                     // 28 DMA1_Channel2_IRQHandler
   Default_Handler,                     // 29 DMA1_Channel3_IRQHandler
   Default_Handler,                     // 30 DMA1_Channel4_IRQHandler
   Default_Handler,                     // 31 DMA1_Channel5_IRQHandler
   Default_Handler,                     // 32 DMA1_Channel6_IRQHandler
   Default_Handler,                     // 33 DMA1_Channel7_IRQHandler
   Default_Handler,                     // 34 ADC1_IRQHandler
   Default_Handler,                     // 35 USB_HP_IRQHandler
   Default_Handler,                     // 36 USB_LP_IRQHandler
   Default_Handler,                     // 37 DAC_IRQHandler
   Default_Handler,                     // 38 COMP_IRQHandler
   Default_Handler,                     // 39 EXTI9_5_IRQHandler
   Default_Handler,                     // 40 LCD_IRQHandler
   Default_Handler,                     // 41 TIM9_IRQHandler
   Default_Handler,                     // 42 TIM10_IRQHandler
   Default_Handler,                     // 43 TIM11_IRQHandler
   Default_Handler,                     // 44 TIM2_IRQHandler
   Default_Handler,                     // 45 TIM3_IRQHandler
   Default_Handler,                     // 46 TIM4_IRQHandler
   Default_Handler,                     // 47 I2C1_EV_IRQHandler
   Default_Handler,                     // 48 I2C1_ER_IRQHandler
   Default_Handler,                     // 49 I2C2_EV_IRQHandler
   Default_Handler,                     // 50 I2C2_ER_IRQHandler
   Default_Handler,                     // 51 SPI1_IRQHandler
   Default_Handler,                     // 52 SPI2_IRQHandler
   Default_Handler,                     // 53 USART1_IRQHandler
   Default_Handler,                     // 54 USART2_IRQHandler
   Default_Handler,                     // 55 USART3_IRQHandler
   Default_Handler,                     // 56 EXTI15_10_IRQHandler
   Default_Handler,                     // 57 RTC_Alarm_IRQHandler
   Default_Handler,                     // 58 USB_FS_WKUP_IRQHandler
   Default_Handler,                     // 59 TIM6_IRQHandler
   Default_Handler,                     // 60 TIM7_IRQHandler
   0,                                   // 61
   0,                                   // 62
   0,                                   // 63
   0,                                   // 64
   0,                                   // 65
   0,                                   // 66 BootRAM
};

/*================================= public ==================================*/

void
NMI_Handler (void)
{
    Reset_Handler();
    while(1)
    {
    }
}

void
HardFault_Handler (void)
{
    while(1)
    {
    }
}

void
Default_Handler (void)
{
    while(1)
    {
    }
}

void
Reset_Handler (void)
{
	uint32_t *pui32Src, *pui32Dest;

    //
	// Copy the data segment initializers from flash to SRAM.
    //
	pui32Src = &_etext;
	for(pui32Dest = &_data; pui32Dest < &_edata; )
	{
		*pui32Dest++ = *pui32Src++;
	}
    
    //
    // Zero fill the bss segment.
    //
    __asm(  "    ldr     r0, =_bss\n"
            "    ldr     r1, =_ebss\n"
            "    mov     r2, #0\n"
            "    .thumb_func\n"
            "    zero_loop:\n"
            "    cmp     r0, r1\n"
            "    it      lt\n"
            "    strlt   r2, [r0], #4\n"
            "    blt     zero_loop"
    );
    
    //
    // Zero fill the bss segment.
    //
    SystemInit();

    //
    // Initialize standard C library
    //
    __libc_init_array();

   //
   // Call the application's entry point.
   //
   main();
}

/*================================ private ==================================*/

