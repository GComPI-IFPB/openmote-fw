/*
 * Copyright 2013 OpenMote Technologies, S.L.
 */

/**
 *
 * @file       UartDriver.h
 * @author     Pere Tuset-Peiro (peretuset@openmote.com)
 * @version    v0.1
 * @date       May, 2014
 * @brief
 * @ingroup
 *
 */

#ifndef UART_DRIVER_H_
#define UART_DRIVER_H_

#include "Uart.h"

#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"

class UartDriver : public Uart
{
public:
    UartDriver(uint32_t peripheral_, uint32_t base_, uint32_t clock_, \
               uint32_t interrupt_, GpioUart& rx_, GpioUart& tx_);
    void rxLock(void);
    void rxUnlock(void);
    void rxUnlockFromInterrupt(void);
    void txLock(void);
    void txUnlock(void);
    void txUnlockFromInterrupt(void);
private:
    SemaphoreHandle_t rxMutex;
    BaseType_t rxHigherPriorityTaskWoken;
    SemaphoreHandle_t txMutex;
    BaseType_t txHigherPriorityTaskWoken;
};

#endif /* UART_DRIVER_H_ */
