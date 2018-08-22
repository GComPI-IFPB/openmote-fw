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

#include "board.h"
#include "platform_types.h"

#include "Board.h"
#include "Gpio.h"
#include "Spi.h"

#include "Callback.h"
#include "Scheduler.h"
#include "Semaphore.h"
#include "Task.h"

/*================================ define ===================================*/

#define GREEN_LED_TASK_PRIORITY             ( tskIDLE_PRIORITY + 0 )
#define TRANSMIT_TASK_PRIORITY              ( tskIDLE_PRIORITY + 1 )

#define GREEN_LED_TASK_STACK_SIZE           ( 128 )
#define TRANSMIT_TASK_STACK_SIZE            ( 1024 )

/*================================ typedef ==================================*/

/*=============================== prototypes ================================*/

extern "C" TickType_t board_sleep(TickType_t xModifiableIdleTime);
extern "C" TickType_t board_wakeup(TickType_t xModifiableIdleTime);

static void prvGreenLedTask(void *pvParameters);
static void prvTransmitTask(void *pvParameters);

static void radio_tx_init(void);
static void radio_tx_done(void);

/*=============================== variables =================================*/

PlainCallback radio_tx_init_cb(&radio_tx_init);
PlainCallback radio_tx_done_cb(&radio_tx_done);

SemaphoreBinary semaphore(false);

/*================================= public ==================================*/

int main(void)
{
    // Initialize the board
    board.init();

    // Create FreeRTOS tasks
    xTaskCreate(prvGreenLedTask, (const char *) "Green", GREEN_LED_TASK_STACK_SIZE, NULL, GREEN_LED_TASK_PRIORITY, NULL);
    xTaskCreate(prvTransmitTask, (const char *) "Transmit", TRANSMIT_TASK_STACK_SIZE, NULL, TRANSMIT_TASK_PRIORITY, NULL);

    // Start the scheduler
    Scheduler::run();
}

TickType_t board_sleep(TickType_t xModifiableIdleTime)
{
    /* Check if radio is active */
    if (radio.canSleep())
    {
        return xModifiableIdleTime;
    }

    return 0;
}

TickType_t board_wakeup(TickType_t xModifiableIdleTime)
{
    return xModifiableIdleTime;
}

/*================================ private ==================================*/

static void prvTransmitTask(void *pvParameters)
{
    static uint8_t tx_buffer[125];
    static uint16_t tx_buffer_len = sizeof(tx_buffer);

    RadioResult result;

    for (uint16_t i = 0; i < tx_buffer_len; i++)
    {
        tx_buffer[i] = i;
    }

    /* Set radio transmit callbacks */
    radio.setTxCallbacks(&radio_tx_init_cb, &radio_tx_done_cb);

    /* Enable radio */
    radio.enable();
    radio.enableInterrupts();

    // Forever
    while (true) {
        // Turn on red LED
        led_red.on();

        /* Turn on radio */
        radio.on();

        /* Load packet to radio */
        result = radio.loadPacket(tx_buffer, tx_buffer_len);
        if (result == RadioResult_Success)
        {
            /* Transmit packet */
            radio.transmit();
        }

        /* Wait until packet has been transamitted */
        semaphore.take();

        /* Turn off radio */
        radio.off();

        // Turn off red LED
        led_red.off();

        // Delay for 10 seconds
        vTaskDelay(10000 / portTICK_RATE_MS);
    }
}

static void prvGreenLedTask(void *pvParameters)
{
    // Forever
    while (true) {
        // Turn off green LED for 9999 ms
        led_green.off();
        vTaskDelay(9999 / portTICK_RATE_MS);

        // Turn on green LED for 1 ms
        led_green.on();
        vTaskDelay(1 / portTICK_RATE_MS);
    }
}

static void radio_tx_init(void)
{
    led_orange.on();
}

static void radio_tx_done(void)
{
    led_orange.off();
    semaphore.giveFromInterrupt();
}
