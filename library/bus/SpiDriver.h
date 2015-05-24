/**
 * @file       SpiDriver.h
 * @author     Pere Tuset-Peiro (peretuset@openmote.com)
 * @version    v0.1
 * @date       May, 2015
 * @brief
 *
 * @copyright  Copyright 2015, OpenMote Technologies, S.L.
 *             This file is licensed under the GNU General Public License v2.
 */

#ifndef SPI_DRIVER_H_
#define SPI_DRIVER_H_

#include "Spi.h"

#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"

class SpiDriver : public Spi
{
public:
    SpiDriver(uint32_t peripheral, uint32_t base, uint32_t clock, \
         GpioSpi& miso, GpioSpi& mosi, GpioSpi& clk, GpioSpi& ncs);
    void lock(void);
    void unlock(void);
private:
    SemaphoreHandle_t xMutex_;
};

#endif /* SPI_DRIVER_H_ */
