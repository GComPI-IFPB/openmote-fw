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
                       uint32_t interrupt_, Gpio * rx_, uint32_t rx_ioc_,\
                       Gpio * tx_, uint32_t tx_ioc_):
    Uart(peripheral_, base_, clock_, interrupt_, rx_, rx_ioc_, tx_, tx_ioc_)
{
    xMutex = xSemaphoreCreateMutex();
    if (xMutex == NULL) {
        while(true);
    }
}

void UartDriver::lock(void)
{
    xSemaphoreTake(xMutex, portMAX_DELAY);
}

void UartDriver::unlock(void)
{
    xSemaphoreGive(xMutex);
}

/*================================ private ==================================*/

