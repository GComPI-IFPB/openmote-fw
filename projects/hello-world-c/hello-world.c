#include "gpio.h"
#include "hw_types.h"
#include "hw_memmap.h"

#define BSP_LED_BASE            GPIO_C_BASE
#define BSP_LED_1               GPIO_PIN_4      //!< PC4 -- red
#define BSP_LED_2               GPIO_PIN_5      //!< PC5 -- orange
#define BSP_LED_3               GPIO_PIN_6      //!< PC6 -- yellow
#define BSP_LED_4               GPIO_PIN_7      //!< PC7 -- green

#define BSP_LED_ALL             (BSP_LED_1 | \
                                 BSP_LED_2 | \
                                 BSP_LED_3 | \
                                 BSP_LED_4)     //!< Bitmask of all LEDs

#define TICKS       ( 160000 )
#define LED_RED     ( BSP_LED_1 )
#define LED_GREEN   ( BSP_LED_4 )

int main(void) {
    volatile uint32_t i = 0;
    
    GPIOPinTypeGPIOOutput(BSP_LED_BASE, BSP_LED_ALL);
    GPIOPinWrite(BSP_LED_BASE, BSP_LED_ALL, 0);
    
    while(1) {
        GPIOPinWrite(BSP_LED_BASE, LED_GREEN, LED_GREEN);
        GPIOPinWrite(BSP_LED_BASE, LED_RED, 0);
        for(i = 0; i < TICKS; i++) {
        }
        GPIOPinWrite(BSP_LED_BASE, LED_GREEN, 0);
        GPIOPinWrite(BSP_LED_BASE, LED_RED, LED_RED); 
        for(i = 0; i < TICKS; i++) {
        }
    }

    return 0;
}

