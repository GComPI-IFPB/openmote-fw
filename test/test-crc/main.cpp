/**
 * @file       main.cpp
 * @author     Pere Tuset-Peiro (peretuset@openmote.com)
 * @version    v0.1
 * @date       May, 2015
 * @brief
 *
 * @copyright  Copyright 2015, OpenMote Technologies, S.L.
 *             This file is licensed under the GNU General Public License v2.
 */

/*================================ include ==================================*/

#include "FreeRTOS.h"
#include "task.h"

#include "openmote-cc2538.h"

#include "Board.h"
#include "Gpio.h"
#include "Spi.h"

#include "Tps62730.h"

#include "Crc16.h"
#include "Scheduler.h"
#include "Semaphore.h"
#include "Task.h"

/*================================ define ===================================*/

#define GREEN_LED_TASK_PRIORITY             ( tskIDLE_PRIORITY + 1 )
#define CRC_TASK_PRIORITY                   ( tskIDLE_PRIORITY + 0 )

#define SPI_BAUDRATE                        ( 8000000 )

/*================================ typedef ==================================*/

/*=============================== prototypes ================================*/

static void prvGreenLedTask(void *pvParameters);
static void prvCrcTask(void *pvParameters);

/*=============================== variables =================================*/

static Crc16 crc16;

/*================================= public ==================================*/

int main (void)
{
    // Set the TPS62730 in bypass mode (Vin = 3.3V, Iq < 1 uA)
    tps62730.setBypass();

    // Enable the SPI peripheral
    spi.enable(SPI_BAUDRATE);

    // Create two FreeRTOS tasks
    xTaskCreate(prvGreenLedTask, (const char *) "Green", 128, NULL, GREEN_LED_TASK_PRIORITY, NULL);
    xTaskCreate(prvCrcTask, (const char *) "Crc", 128, NULL, CRC_TASK_PRIORITY, NULL);

    // Start the scheduler
    Scheduler::run();
}

/*=============================== protected =================================*/

/*================================ private ==================================*/

static void prvCrcTask(void *pvParameters)
{
    uint8_t crc_test[9] = {0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39};
    uint16_t crc_output;
    uint8_t * ptr = crc_test;
    uint8_t size = sizeof(crc_test);

    // Forever
    while(true)
    {
        ptr = crc_test;
        size = sizeof(crc_test);

        crc16.init();
        while(size--) {
            crc16.set(*ptr++);
        }
        crc_output = crc16.get();

        uint8_t high = (crc_output >> 8) & 0xFF;
        uint8_t low = (crc_output >> 0) & 0xFF;

        spi.rwByte(high);
        spi.rwByte(low);

        crc16.set(high);
        crc16.set(low);

        crc_output = crc16.get();

        high = (crc_output >> 8) & 0xFF;
        low = (crc_output >> 0) & 0xFF;

        spi.rwByte(high);
        spi.rwByte(low);

        if (crc16.check())
        {
            led_yellow.on();
        }
        else {
            led_red.on();
        }

        Task::delay(250);

        led_red.off();
        led_yellow.off();

        Task::delay(250);
    }
}

static void prvGreenLedTask(void *pvParameters)
{
    // Forever
    while (true)
    {
        // Turn off green LED for 950 ms
        led_green.off();
        Task::delay(950);

        // Turn on green LED for 50 ms
        led_green.on();
        Task::delay(50);

    }
}
