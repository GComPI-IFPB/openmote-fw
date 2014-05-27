#include <stdint.h> 
#include "FreeRTOS.h"
#include "task.h"

#include "gpio.h"
#include "hw_types.h"
#include "hw_memmap.h"

//openmote_cc2538
#define BSP_LED_BASE            GPIO_C_BASE
#define BSP_LED_1               GPIO_PIN_4      //!< PC4 -- red
#define BSP_LED_2               GPIO_PIN_5      //!< PC5 -- orange
#define BSP_LED_3               GPIO_PIN_6      //!< PC6 -- yellow
#define BSP_LED_4               GPIO_PIN_7      //!< PC7 -- green

#define BSP_LED_ALL             (BSP_LED_1 | \
                                 BSP_LED_2 | \
                                 BSP_LED_3 | \
                                 BSP_LED_4)     //!< Bitmask of all LEDs

void HelloWorld(void* param) {
    uint8_t status;

    while(1)
    {
        status = GPIOPinRead(BSP_LED_BASE, BSP_LED_1);
        GPIOPinWrite(BSP_LED_BASE, BSP_LED_1, status);
        status != status;
        vTaskDelay(1000);
    }
}

int main(void) {

    GPIOPinTypeGPIOOutput(BSP_LED_BASE, BSP_LED_ALL);
    GPIOPinWrite(BSP_LED_BASE, BSP_LED_ALL, 0);
    
    xTaskCreate(HelloWorld, (const signed char*)"HelloWorld", 0, NULL, 1, 0);
    vTaskStartScheduler();

    return 0;
}

