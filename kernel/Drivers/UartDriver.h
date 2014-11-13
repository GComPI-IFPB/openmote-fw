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
    UartDriver(uint32_t peripheral, uint32_t base, uint32_t clock, \
               uint32_t interrupt, GpioUart& rx, GpioUart& tx);
    void rxLock(void);
    void rxUnlock(void);
    void rxUnlockFromInterrupt(void);
    void txLock(void);
    void txUnlock(void);
    void txUnlockFromInterrupt(void);
private:
    SemaphoreHandle_t rxMutex_;
    BaseType_t rxHigherPriorityTaskWoken_;
    SemaphoreHandle_t txMutex_;
    BaseType_t txHigherPriorityTaskWoken_;
};

#endif /* UART_DRIVER_H_ */
