/*
 * Copyright 2013 OpenMote Technologies, S.L.
 */

/**
 *
 * @file       UartDriver.cpp
 * @author     Pere Tuset-Peiro (peretuset@openmote.com)
 * @version    v0.1
 * @date       May, 2014
 * @brief
 * @ingroup
 *
 */

/*================================ include ==================================*/

#include "UartDriver.h"

/*================================ define ===================================*/

/*================================ typedef ==================================*/

/*=============================== variables =================================*/

/*=============================== prototypes ================================*/

/*================================= public ==================================*/

UartDriver::UartDriver(uint32_t peripheral_, uint32_t base_, uint32_t clock_, \
                       uint32_t interrupt_, GpioUart& rx_, GpioUart& tx_):
    Uart(peripheral_, base_, clock_, interrupt_, rx_, tx_)
{
    rxMutex = xSemaphoreCreateMutex();
    if (rxMutex == NULL) {
        while(true);
    }

    txMutex = xSemaphoreCreateMutex();
    if (txMutex == NULL) {
        while(true);
    }
}

void UartDriver::rxLock(void)
{
    xSemaphoreTake(rxMutex, portMAX_DELAY);
}

void UartDriver::rxUnlock(void)
{
    xSemaphoreGive(rxMutex);
}

void UartDriver::txLock(void)
{
    xSemaphoreTake(txMutex, portMAX_DELAY);
}

void UartDriver::txUnlock(void)
{
    xSemaphoreGive(txMutex);
}

void UartDriver::rxUnlockFromInterrupt(void)
{
    rxHigherPriorityTaskWoken = pdFALSE;
    xSemaphoreGiveFromISR(rxMutex, &rxHigherPriorityTaskWoken);
    portYIELD_FROM_ISR(rxHigherPriorityTaskWoken);
}

void UartDriver::txUnlockFromInterrupt(void)
{
    txHigherPriorityTaskWoken = pdFALSE;
    xSemaphoreGiveFromISR(txMutex, &txHigherPriorityTaskWoken);
    portYIELD_FROM_ISR(txHigherPriorityTaskWoken);
}

/*=============================== protected =================================*/

/*================================ private ==================================*/
