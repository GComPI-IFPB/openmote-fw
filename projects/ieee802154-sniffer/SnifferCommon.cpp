/**
 * @file       SnifferCommon.cpp
 * @author     Pere Tuset-Peiro (peretuset@openmote.com)
 * @version    v0.1
 * @date       May, 2015
 * @brief
 *
 * @copyright  Copyright 2015, OpenMote Technologies, S.L.
 *             This file is licensed under the GNU General Public License v2.
 */

/*================================ include ==================================*/

#include <string.h>

#include "SnifferCommon.h"
#include "Gpio.h"

#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"

/*================================ define ===================================*/

/*================================ typedef ==================================*/

/*=============================== prototypes ================================*/

/*=============================== variables =================================*/

extern GpioOut led_red;
extern GpioOut led_orange;

/*================================= public ==================================*/

SnifferCommon::SnifferCommon(Board& board, Radio& radio):
    board_(board), radio_(radio), \
    snifferRadioRxInitCallback_(this, &SnifferCommon::radioRxInitCallback), \
    snifferRadioRxDoneCallback_(this, &SnifferCommon::radioRxDoneCallback), \
    radioBuffer_ptr(radioBuffer), radioBuffer_len(sizeof(radioBuffer))
{
}

void SnifferCommon::init(void)
{
    // Create the receive semaphore
    radioRxSemaphore = xSemaphoreCreateMutex();

    // Take the receive semaphore so that we block until a packet is received
    xSemaphoreTake(radioRxSemaphore, (TickType_t) portMAX_DELAY);

    // Set Radio receive callbacks
    radio_.setRxCallbacks(&snifferRadioRxInitCallback_, \
                          &snifferRadioRxDoneCallback_);

    // Enable Radio module
    radio_.enable();
    radio_.enableInterrupts();
}

void SnifferCommon::start(void)
{
    // Start receiving
    radio_.on();
    radio_.receive();
    led_orange.on();
}

void SnifferCommon::stop(void)
{
    // Stop receiving
    radio_.off();
    led_orange.off();
}

void SnifferCommon::setChannel(uint8_t channel)
{
    // Set the radio channel
    radio_.setChannel(channel);
}

/*================================ private ==================================*/

void SnifferCommon::radioRxInitCallback(void)
{
    led_red.on();
}

void SnifferCommon::radioRxDoneCallback(void)
{
    // Determines if the interrupt triggers a context switch
    xHigherPriorityTaskWoken = pdFALSE;

    led_red.off();

    // Give the receive semaphore as the packet has been received
    xSemaphoreGiveFromISR(radioRxSemaphore, &xHigherPriorityTaskWoken);

    // Force a context switch after the interrupt if required
    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}
