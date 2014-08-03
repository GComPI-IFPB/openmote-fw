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

#include "Uart.h"

#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"

class UartDriver : public Uart
{

public:
    UartDriver(uint32_t peripheral_, uint32_t base_, uint32_t clock_, \
               uint32_t interrupt_, Gpio * rx_, uint32_t rx_ioc_,\
               Gpio * tx_, uint32_t tx_ioc_);
    void lock(void);
    void unlock(void);
private:
    SemaphoreHandle_t xMutex;
};
