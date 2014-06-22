#include <stdint.h>


//*****************************************************************************
//
// Extern and local function prototypes.
//
//*****************************************************************************
extern int main (void);
extern void __libc_init_array(void);

void ResetISR(void);
void NmiISR(void);
void FaultISR(void);
void IntDefaultHandler(void);

static uint32_t pui32Stack[128];

__attribute__ ((section(".vectors"), used))
void (* const gVectors[])(void) =
{
   (void (*)(void))((uint32_t)pui32Stack + sizeof(pui32Stack)), // Stack pointer
   ResetISR,							   // Reset handler
   NmiISR,                                 // The NMI handler
   FaultISR,                               // The hard fault handler
   IntDefaultHandler,                      // 4 The MPU fault handler
   IntDefaultHandler,                      // 5 The bus fault handler
   IntDefaultHandler,                      // 6 The usage fault handler
   0,                                      // 7 Reserved
   0,                                      // 8 Reserved
   0,                                      // 9 Reserved
   0,                                      // 10 Reserved
   IntDefaultHandler,                        // 11 SVCall handler
   IntDefaultHandler,                      // 12 Debug monitor handler
   0,                                      // 13 Reserved
   IntDefaultHandler,                     // 14 The PendSV handler
   IntDefaultHandler,                    // 15 The SysTick handler
   IntDefaultHandler,                      // 16 GPIO Port A
   IntDefaultHandler,                      // 17 GPIO Port B
   IntDefaultHandler,                      // 18 GPIO Port C
   IntDefaultHandler,                      // 19 GPIO Port D
   0,                                      // 20 none
   IntDefaultHandler,                      // 21 UART0 Rx and Tx
   IntDefaultHandler,                      // 22 UART1 Rx and Tx
   IntDefaultHandler,                      // 23 SSI0 Rx and Tx
   IntDefaultHandler,                      // 24 I2C Master and Slave
   0,                                      // 25 Reserved
   0,                                      // 26 Reserved
   0,                                      // 27 Reserved
   0,                                      // 28 Reserved
   0,                                      // 29 Reserved
   IntDefaultHandler,                      // 30 ADC Sequence 0
   0,                                      // 31 Reserved
   0,                                      // 32 Reserved
   0,                                      // 33 Reserved
   IntDefaultHandler,                      // 34 Watchdog timer, timer 0
   IntDefaultHandler,                      // 35 Timer 0 subtimer A
   IntDefaultHandler,                      // 36 Timer 0 subtimer B
   IntDefaultHandler,                      // 37 Timer 1 subtimer A
   IntDefaultHandler,                      // 38 Timer 1 subtimer B
   IntDefaultHandler,                      // 39 Timer 2 subtimer A
   IntDefaultHandler,                      // 40 Timer 2 subtimer B
   IntDefaultHandler,                      // 41 Analog Comparator 0
   IntDefaultHandler,                      // 42 RFCore Rx/Tx
   IntDefaultHandler,                      // 43 RFCore Error
   IntDefaultHandler,                      // 44 IcePick
   IntDefaultHandler,                      // 45 FLASH Control
   IntDefaultHandler,                      // 46 AES
   IntDefaultHandler,                      // 47 PKA
   IntDefaultHandler,                      // 48 Sleep Timer
   IntDefaultHandler,                      // 49 MacTimer
   IntDefaultHandler,                      // 50 SSI1 Rx and Tx
   IntDefaultHandler,                      // 51 Timer 3 subtimer A
   IntDefaultHandler,                      // 52 Timer 3 subtimer B
   0,                                      // 53 Reserved
   0,                                      // 54 Reserved
   0,                                      // 55 Reserved
   0,                                      // 56 Reserved
   0,                                      // 57 Reserved
   0,                                      // 58 Reserved
   0,                                      // 59 Reserved
   IntDefaultHandler,                      // 60 USB 2538
   0,                                      // 61 Reserved
   IntDefaultHandler,                      // 62 uDMA
   IntDefaultHandler,                      // 63 uDMA Error
};

//*****************************************************************************
//
// The following are constructs created by the linker, indicating where the
// the "data" and "bss" segments reside in memory.  The initializers for the
// for the "data" segment resides immediately following the "text" segment.
//
//*****************************************************************************
extern uint32_t _etext;
extern uint32_t _data;
extern uint32_t _edata;
extern uint32_t _bss;
extern uint32_t _ebss;

//
// And here are the weak interrupt handlers.
//
void
NmiISR (void)
{
    ResetISR();
    while(1)
    {
    }
}

void
FaultISR (void)
{
    while(1)
    {
    }
}

void
IntDefaultHandler (void)
{
    while(1)
    {
    }
}

void
ResetISR (void)
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
    for(pui32Dest = &_bss; pui32Dest < &_ebss; )
    {
        *pui32Dest++ = 0;
    }

    //
    // Initialize standard C library
    //
    __libc_init_array();


   //
   // Call the application's entry point.
   //
   main();
}
