/**
 * @file       UartDriver.cpp
 * @author     Pere Tuset-Peiro (peretuset@openmote.com)
 * @version    v0.1
 * @date       May, 2015
 * @brief
 *
 * @copyright  Copyright 2015, OpenMote Technologies, S.L.
 *             This file is licensed under the GNU General Public License v2.
 */

/*================================ include ==================================*/

#include "UartDriver.h"

/*================================ define ===================================*/

/*================================ typedef ==================================*/

/*=============================== variables =================================*/

/*=============================== prototypes ================================*/

/*================================= public ==================================*/

UartDriver::UartDriver(uint32_t peripheral, uint32_t base, uint32_t clock, \
                       uint32_t interrupt, GpioUart& rx, GpioUart& tx):
    Uart(peripheral, base, clock, interrupt, rx, tx)
{
    rxMutex_ = xSemaphoreCreateMutex();
    if (rxMutex_ == NULL) {
        while(true);
    }

    txMutex_ = xSemaphoreCreateMutex();
    if (txMutex_ == NULL) {
        while(true);
    }
}

void UartDriver::rxLock(void)
{
    xSemaphoreTake(rxMutex_, portMAX_DELAY);
}

void UartDriver::rxUnlock(void)
{
    xSemaphoreGive(rxMutex_);
}

void UartDriver::txLock(void)
{
    xSemaphoreTake(txMutex_, portMAX_DELAY);
}

void UartDriver::txUnlock(void)
{
    xSemaphoreGive(txMutex_);
}

void UartDriver::rxUnlockFromInterrupt(void)
{
    rxHigherPriorityTaskWoken_ = pdFALSE;
    xSemaphoreGiveFromISR(rxMutex_, &rxHigherPriorityTaskWoken_);
    portYIELD_FROM_ISR(rxHigherPriorityTaskWoken_);
}

void UartDriver::txUnlockFromInterrupt(void)
{
    txHigherPriorityTaskWoken_ = pdFALSE;
    xSemaphoreGiveFromISR(txMutex_, &txHigherPriorityTaskWoken_);
    portYIELD_FROM_ISR(txHigherPriorityTaskWoken_);
}

/*=============================== protected =================================*/

/*================================ private ==================================*/
