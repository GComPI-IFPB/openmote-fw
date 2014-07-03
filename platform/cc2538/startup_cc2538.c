/*
 * Copyright 2013 OpenMote Technologies, S.L.
 */

/**
 *
 * @file       startup_cc2538.c
 * @author     Pere Tuset-Peiro (peretuset@openmote.com)
 * @version    v0.1
 * @date       May, 2014
 * @brief
 * @ingroup
 *
 */

/*================================ include ==================================*/

#include <stdint.h>

#include "hw_nvic.h"

/*================================ define ===================================*/

#define FLASH_START_ADDR                0x00200000
#define BOOTLOADER_BACKDOOR_ENABLED     0xF6FFFFFF // ENABLED: PORT A, PIN 6, LOW
#define BOOTLOADER_BACKDOOR_DISABLED    0xEFFFFFFF // DISABLED
#define SYS_CTRL_EMUOVR                 0x400D20B4
#define SYS_CTRL_I_MAP                  0x400D2098

#ifndef HWREG
#define HWREG(x)                                                              \
        (*((volatile unsigned long *)(x)))
#endif

/*================================ typedef ==================================*/

typedef struct
{
    uint32_t ui32BootldrCfg;
    uint32_t ui32ImageValid;
    uint32_t ui32ImageVectorAddr;
} lockPageCCA_t;

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

extern void SleepTimerHandler(void);

/*=============================== variables =================================*/

static uint32_t pui32Stack[128];

extern uint32_t _text;
extern uint32_t _etext;
extern uint32_t _data;
extern uint32_t _edata;
extern uint32_t _bss;
extern uint32_t _ebss;

__attribute__ ((section(".flashcca"), used))
const lockPageCCA_t __cca =
{
  BOOTLOADER_BACKDOOR_ENABLED,  // Bootloader backdoor enabled
  0,               				// Image valid bytes
  FLASH_START_ADDR 				// Vector table located at flash start address
};

__attribute__ ((section(".vectors"), used))
void (* const gVectors[])(void) =
{
   (void (*)(void))((uint32_t)pui32Stack + sizeof(pui32Stack)), // Stack pointer
   Reset_Handler,                       // Reset handler
   NMI_Handler,                         // The NMI handler
   HardFault_Handler,                   // The hard fault handler
   Default_Handler,                     // 4 The MPU fault handler
   Default_Handler,                     // 5 The bus fault handler
   Default_Handler,                     // 6 The usage fault handler
   0,                                   // 7 Reserved
   0,                                   // 8 Reserved
   0,                                   // 9 Reserved
   0,                                   // 10 Reserved
   vPortSVCHandler,                     // 11 SVCall handler
   Default_Handler,                     // 12 Debug monitor handler
   0,                                   // 13 Reserved
   xPortPendSVHandler,                  // 14 The PendSV handler
   xPortSysTickHandler,                 // 15 The SysTick handler
   Default_Handler,                     // 16 GPIO Port A
   Default_Handler,                     // 17 GPIO Port B
   Default_Handler,                     // 18 GPIO Port C
   Default_Handler,                     // 19 GPIO Port D
   0,                                   // 20 none
   Default_Handler,                     // 21 UART0 Rx and Tx
   Default_Handler,                     // 22 UART1 Rx and Tx
   Default_Handler,                     // 23 SSI0 Rx and Tx
   Default_Handler,                     // 24 I2C Master and Slave
   0,                                   // 25 Reserved
   0,                                   // 26 Reserved
   0,                                   // 27 Reserved
   0,                                   // 28 Reserved
   0,                                   // 29 Reserved
   Default_Handler,                     // 30 ADC Sequence 0
   0,                                   // 31 Reserved
   0,                                   // 32 Reserved
   0,                                   // 33 Reserved
   Default_Handler,                     // 34 Watchdog timer, timer 0
   Default_Handler,                     // 35 Timer 0 subtimer A
   Default_Handler,                     // 36 Timer 0 subtimer B
   Default_Handler,                     // 37 Timer 1 subtimer A
   Default_Handler,                     // 38 Timer 1 subtimer B
   Default_Handler,                     // 39 Timer 2 subtimer A
   Default_Handler,                     // 40 Timer 2 subtimer B
   Default_Handler,                     // 41 Analog Comparator 0
   Default_Handler,                     // 42 RFCore Rx/Tx
   Default_Handler,                     // 43 RFCore Error
   Default_Handler,                     // 44 IcePick
   Default_Handler,                     // 45 FLASH Control
   Default_Handler,                     // 46 AES
   Default_Handler,                     // 47 PKA
   Default_Handler,                     // 48 Sleep Timer
   Default_Handler,                     // 49 MacTimer
   Default_Handler,                     // 50 SSI1 Rx and Tx
   Default_Handler,                     // 51 Timer 3 subtimer A
   Default_Handler,                     // 52 Timer 3 subtimer B
   0,                                   // 53 Reserved
   0,                                   // 54 Reserved
   0,                                   // 55 Reserved
   0,                                   // 56 Reserved
   0,                                   // 57 Reserved
   0,                                   // 58 Reserved
   0,                                   // 59 Reserved
   Default_Handler,                     // 60 USB
   0,                                   // 61 Reserved
   Default_Handler,                     // 62 uDMA
   Default_Handler,                     // 63 uDMA Error
#ifndef CC2538_USE_ALTERNATE_INTERRUPT_MAP
   0,                                   // 64 64-155 are not in use
   0,                                   // 65
   0,                                   // 66
   0,                                   // 67
   0,                                   // 68
   0,                                   // 69
   0,                                   // 70
   0,                                   // 71
   0,                                   // 72
   0,                                   // 73
   0,                                   // 74
   0,                                   // 75
   0,                                   // 76
   0,                                   // 77
   0,                                   // 78
   0,                                   // 79
   0,                                   // 80
   0,                                   // 81
   0,                                   // 82
   0,                                   // 83
   0,                                   // 84
   0,                                   // 85
   0,                                   // 86
   0,                                   // 87
   0,                                   // 88
   0,                                   // 89
   0,                                   // 90
   0,                                   // 91
   0,                                   // 92
   0,                                   // 93
   0,                                   // 94
   0,                                   // 95
   0,                                   // 96
   0,                                   // 97
   0,                                   // 98
   0,                                   // 99
   0,                                   // 100
   0,                                   // 101
   0,                                   // 102
   0,                                   // 103
   0,                                   // 104
   0,                                   // 105
   0,                                   // 106
   0,                                   // 107
   0,                                   // 108
   0,                                   // 109
   0,                                   // 110
   0,                                   // 111
   0,                                   // 112
   0,                                   // 113
   0,                                   // 114
   0,                                   // 115
   0,                                   // 116
   0,                                   // 117
   0,                                   // 118
   0,                                   // 119
   0,                                   // 120
   0,                                   // 121
   0,                                   // 122
   0,                                   // 123
   0,                                   // 124
   0,                                   // 125
   0,                                   // 126
   0,                                   // 127
   0,                                   // 128
   0,                                   // 129
   0,                                   // 130
   0,                                   // 131
   0,                                   // 132
   0,                                   // 133
   0,                                   // 134
   0,                                   // 135
   0,                                   // 136
   0,                                   // 137
   0,                                   // 138
   0,                                   // 139
   0,                                   // 140
   0,                                   // 141
   0,                                   // 142
   0,                                   // 143
   0,                                   // 144
   0,                                   // 145
   0,                                   // 146
   0,                                   // 147
   0,                                   // 148
   0,                                   // 149
   0,                                   // 150
   0,                                   // 151
   0,                                   // 152
   0,                                   // 153
   0,                                   // 154
   0,                                   // 155
   Default_Handler,                     // 156 USB
   Default_Handler,                     // 157 RFCORE RX/TX
   Default_Handler,                     // 158 RFCORE Error
   Default_Handler,                     // 159 AES
   Default_Handler,                     // 160 PKA
   SleepTimerHandler,                   // 161 SMTimer
   Default_Handler,                     // 162 MACTimer
#endif
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
	// Workaround for PM debug issue
    //
    HWREG(SYS_CTRL_EMUOVR) = 0xFF;

    //
    // Workaround for J-Link debug issue
    //
    HWREG(NVIC_VTABLE) = (uint32_t)gVectors;

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
    // Initialize standard C library
    //
    __libc_init_array();

#ifdef CC2538_USE_ALTERNATE_INTERRUPT_MAP
    //
    // Enable alternate interrupt mapping
    //
    HWREG(SYS_CTRL_I_MAP) |= 1;
#endif

   //
   // Call the application's entry point.
   //
   main();

   //
   // End here if return from main()
   //
   while(1)
   {
   }
}

/*================================ private ==================================*/
