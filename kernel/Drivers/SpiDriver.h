/*
 * Copyright 2013 OpenMote Technologies, S.L.
 */

/**
 *
 * @file       SpiDriver.h
 * @author     Pere Tuset-Peiro (peretuset@openmote.com)
 * @version    v0.1
 * @date       May, 2014
 * @brief
 * @ingroup
 *
 */

#include "Spi.h"

#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"

class SpiDriver : public Spi
{

public:
    SpiDriver(uint32_t peripheral_, uint32_t base_, uint32_t clock_, \
         Gpio* miso_, Gpio* mosi_, Gpio* clk_, Gpio* ncs_);
    void lock(void);
    void unlock(void);
private:
    SemaphoreHandle_t xMutex;
};
