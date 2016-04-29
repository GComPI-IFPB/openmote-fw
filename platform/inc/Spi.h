/**
 * @file       Spi.h
 * @author     Pere Tuset-Peiro (peretuset@openmote.com)
 * @version    v0.1
 * @date       May, 2015
 * @brief
 *
 * @copyright  Copyright 2015, OpenMote Technologies, S.L.
 *             This file is licensed under the GNU General Public License v2.
 */

#ifndef SPI_H_
#define SPI_H_

#include <stdint.h>

#include "Callback.h"
#include "Mutex.h"

class Gpio;
struct SpiConfig;

class Spi
{

friend class InterruptHandler;

public:
    Spi(Gpio& miso, Gpio& mosi, Gpio& clk, GpioOut& ncs, SpiConfig& config);
    void enable(uint32_t baudrate = 0);
    void sleep(void);
    void wakeup(void);
    void setRxCallback(Callback* callback);
    void setTxCallback(Callback* callback);
    void enableInterrupts(void);
    void disableInterrupts(void);
    void select(void);
    void deselect(void);
    uint8_t readByte(void);
    uint32_t readByte(uint8_t * buffer, uint32_t length);
    void writeByte(uint8_t byte);
    uint32_t writeByte(uint8_t * buffer, uint32_t length);
protected:
    uint32_t getBase(void);
    void interruptHandler(void);
private:
    void interruptHandlerRx();
    void interruptHandlerTx();
private:
    Gpio& miso_;
    Gpio& mosi_;
    Gpio& clk_;
    GpioOut& ncs_;

    SpiConfig& config_;

    Callback* rx_callback_;
    Callback* tx_callback_;
};

#endif /* SPI_H_ */
