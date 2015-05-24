/**
 * @file       cc2538_startup.c
 * @author     Pere Tuset-Peiro (peretuset@openmote.com)
 * @version    v0.1
 * @date       May, 2015
 * @brief
 *
 * @copyright  Copyright 2015, OpenMote Technologies, S.L.
 *             This file is licensed under the GNU General Public License v2.
 */

/*================================ include ==================================*/

#include <stdint.h>

#include "cc2538_include.h"

/*================================ define ===================================*/

#define STACK_SIZE                      ( 1024 )

#define FLASH_START_ADDR                ( 0x00200000 )
#define BOOTLOADER_BACKDOOR_ENABLED     ( 0xF6FFFFFF ) // ENABLED: PORT A, PIN 6, LOW
#define BOOTLOADER_BACKDOOR_DISABLED    ( 0xEFFFFFFF ) // DISABLED

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

static void system_init(void);

void Reset_Handler(void);
void NMI_Handler(void);
void HardFault_Handler(void);
void Default_Handler(void);

extern int main (void);

extern void __libc_init_array(void);

extern void vPortSVCHandler(void);
extern void xPortPendSVHandler(void);
extern void xPortSysTickHandler(void);

/*=============================== variables =================================*/

extern uint32_t _text_start;
extern uint32_t _text_end;
extern uint32_t _data_start;
extern uint32_t _data_end;
extern uint32_t _bss_start;
extern uint32_t _bss_end;

__attribute__ ((section(".stack"), used))
static uint8_t _stack[STACK_SIZE];

__attribute__ ((section(".flashcca"), used))
const lockPageCCA_t _cca =
{
  BOOTLOADER_BACKDOOR_ENABLED,  // Bootloader backdoor enabled
  0,               				// Image valid bytes
  FLASH_START_ADDR 				// Vector table located at flash start address
};

__attribute__ ((section(".vectors"), used))
void (* const gVectors[])(void) =
{
   (void (*)(void))((uint32_t)&_stack + sizeof(_stack)), // Stack pointer
   Reset_Handler,                                        // Reset handler
   NMI_Handler,                                          // The NMI handler
   HardFault_Handler,                                    // The hard fault handler
   Default_Handler,                                      // 4 The MPU fault handler
   Default_Handler,                                      // 5 The bus fault handler
   Default_Handler,                                      // 6 The usage fault handler
   0, 0, 0, 0,                                           // 7-10 Reserved
   vPortSVCHandler,                                      // 11 SVCall handler
   Default_Handler,                                      // 12 Debug monitor handler
   0,                                                    // 13 Reserved
   xPortPendSVHandler,                                   // 14 The PendSV handler
   xPortSysTickHandler,                                  // 15 The SysTick handler
   Default_Handler,                                      // 16 GPIO Port A
   Default_Handler,                                      // 17 GPIO Port B
   Default_Handler,                                      // 18 GPIO Port C
   Default_Handler,                                      // 19 GPIO Port D
   0,                                                    // 20 Reserved
   Default_Handler,                                      // 21 UART0 Rx and Tx
   Default_Handler,                                      // 22 UART1 Rx and Tx
   Default_Handler,                                      // 23 SSI0 Rx and Tx
   Default_Handler,                                      // 24 I2C Master and Slave
   0, 0, 0, 0, 0,                                        // 25-29 Reserved
   Default_Handler,                                      // 30 ADC Sequence 0
   0, 0, 0,                                              // 31-33 Reserved
   Default_Handler,                                      // 34 Watchdog timer, timer 0
   Default_Handler,                                      // 35 Timer 0 subtimer A
   Default_Handler,                                      // 36 Timer 0 subtimer B
   Default_Handler,                                      // 37 Timer 1 subtimer A
   Default_Handler,                                      // 38 Timer 1 subtimer B
   Default_Handler,                                      // 39 Timer 2 subtimer A
   Default_Handler,                                      // 40 Timer 2 subtimer B
   Default_Handler,                                      // 41 Analog Comparator 0
   Default_Handler,                                      // 42 RFCore Rx/Tx
   Default_Handler,                                      // 43 RFCore Error
   Default_Handler,                                      // 44 IcePick
   Default_Handler,                                      // 45 FLASH Control
   Default_Handler,                                      // 46 AES
   Default_Handler,                                      // 47 PKA
   Default_Handler,                                      // 48 Sleep Timer
   Default_Handler,                                      // 49 MacTimer
   Default_Handler,                                      // 50 SSI1 Rx and Tx
   Default_Handler,                                      // 51 Timer 3 subtimer A
   Default_Handler,                                      // 52 Timer 3 subtimer B
   0, 0, 0, 0, 0, 0, 0,                                  // 53-59 Reserved
   Default_Handler,                                      // 60 USB
   0,                                                    // 61 Reserved
   Default_Handler,                                      // 62 uDMA
   Default_Handler,                                      // 63 uDMA Error
#ifndef CC2538_USE_ALTERNATE_INTERRUPT_MAP
   0, 0, 0, 0, 0, 0, 0, 0, 0, 0,                         // 64-73 Reserved
   0, 0, 0, 0, 0, 0, 0, 0, 0, 0,                         // 74-83 Reserved
   0, 0, 0, 0, 0, 0, 0, 0, 0, 0,                         // 84-93 Reserved
   0, 0, 0, 0, 0, 0, 0, 0, 0, 0,                         // 94-103 Reserved
   0, 0, 0, 0, 0, 0, 0, 0, 0, 0,                         // 104-113 Reserved
   0, 0, 0, 0, 0, 0, 0, 0, 0, 0,                         // 114-123 Reserved
   0, 0, 0, 0, 0, 0, 0, 0, 0, 0,                         // 124-133 Reserved
   0, 0, 0, 0, 0, 0, 0, 0, 0, 0,                         // 134-143 Reserved
   0, 0, 0, 0, 0, 0, 0, 0, 0, 0,                         // 144-143 Reserved
   0, 0,                                                 // 154-155 Reserved
   Default_Handler,                                      // 156 USB
   Default_Handler,                                      // 157 RFCORE RX/TX
   Default_Handler,                                      // 158 RFCORE Error
   Default_Handler,                                      // 159 AES
   Default_Handler,                                      // 160 PKA
   Default_Handler,                                      // 161 SMTimer
   Default_Handler                                       // 162 MACTimer
#endif
};

/*================================= public ==================================*/

void
NMI_Handler(void)
{
    Reset_Handler();
    while(1)
    {
        __asm("nop");
    }
}

void
HardFault_Handler(void)
{
    while(1)
    {
        __asm("nop");
    }
}

void
Default_Handler(void)
{
    while(1)
    {
        __asm("nop");
    }
}

void
Reset_Handler(void)
{
    volatile uint32_t *src, *dst;

    /* Workaround for PM debug issue */
    HWREG(SYS_CTRL_EMUOVR) = 0xFF;

    /* Workaround for J-Link debug issue */
    HWREG(NVIC_VTABLE) = (uint32_t)gVectors;

#ifdef CC2538_USE_ALTERNATE_INTERRUPT_MAP
    /* Enable alternate interrupt mapping */
    HWREG(SYS_CTRL_I_MAP) |= 1;
#endif

    /* Copy the data segment initializers from flash to SRAM */
    for (src = &_text_end, dst = &_data_start; dst < &_data_end; )
    {
        *dst++ = *src++;
    }

    /* Initialize the BSS section to zero */
    for (dst = &_bss_start; dst < &_bss_end; *dst++)
    {
        *dst = 0;
    }

    /* Initialize the system */
    system_init();

    /* Initialize standard C library */
    __libc_init_array();

   /* Call the application's entry point */
   main();

   /* End here if return from main() */
   while(1)
   {
    __asm("nop");
   }
}

/*================================ private ==================================*/

static void system_init(void)
{
    /**
     * Configure the 32 kHz pins, PD6 and PD7, for crystal operation
     * By default they are configured as GPIOs
     */
    GPIODirModeSet(GPIO_D_BASE, 0x40, GPIO_DIR_MODE_IN);
    GPIODirModeSet(GPIO_D_BASE, 0x80, GPIO_DIR_MODE_IN);
    IOCPadConfigSet(GPIO_D_BASE, 0x40, IOC_OVERRIDE_ANA);
    IOCPadConfigSet(GPIO_D_BASE, 0x80, IOC_OVERRIDE_ANA);

    /**
     * Set the real-time clock to use the 32.768 kHz external crystal
     * Set the system clock to use the 32 MHz external crystal
     */
    SysCtrlClockSet(true, false, SYS_CTRL_SYSDIV_32MHZ);

    /**
     * Set the IO clock to operate at 16 MHz
     * This way peripherals can run while the system clock is gated
     */
    SysCtrlIOClockSet(SYS_CTRL_SYSDIV_16MHZ);

    /**
     * Wait until the 32 MHz oscillator becomes stable
     */
    while (!((HWREG(SYS_CTRL_CLOCK_STA)) & (SYS_CTRL_CLOCK_STA_XOSC_STB)));
}

