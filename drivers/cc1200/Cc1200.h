/**
 * @file       Cc1200.h
 * @author     Pere Tuset-Peiro (peretuset@openmote.com)
 * @version    v0.1
 * @date       May, 2016
 * @brief
 *
 * @copyright  Copyright 2016, OpenMote Technologies, S.L.
 *             This file is licensed under the GNU General Public License v2.
 */
#ifndef CC1200_H_
#define CC1200_H_

#include "Radio.h"
#include "Gpio.h"
#include "Spi.h"

#include "Cc1200_regs.h"

typedef struct {
  uint16_t address;
  uint8_t value;
} cc1200_config_pair_t;

class Cc1200 : public Radio
{
public:
    Cc1200(Spi& spi, GpioOut& cs, GpioIn& gpio0, GpioIn& gpio2, GpioIn& gpio3);
    void init(void);
    void configure(cc1200_config_pair_t* config, uint32_t size);
    void sleep(void);
    void wakeup(void);
    void on(void);
    void off(void);
    void idle(void);
    void reset(void);
    void transmit(void);
    void receive(void);
    void calibrate(void);
    void setRxCallbacks(Callback* rxInit, Callback* rxDone);
    void setTxCallbacks(Callback* txInit, Callback* txDone);
    void enableInterrupts(void);
    void disableInterrupts(void);
    void setChannel(uint8_t channel);
    void setPower(uint8_t power);
    RadioResult loadPacket(uint8_t* data, uint8_t length);
    RadioResult getPacket(uint8_t* buffer, uint8_t* length, int8_t* rssi, uint8_t* lqi, uint8_t* crc);
private:
	uint8_t strobe(uint8_t strobe);
	uint8_t state(void);
private:
    uint8_t singleRead(uint16_t address);
    uint8_t singleWrite(uint16_t address, uint8_t value);
private:
    Spi& spi_;
    GpioOut& cs_;

    GpioIn& gpio0_;
    GpioIn& gpio2_;
    GpioIn& gpio3_;
};

#endif /* CC1200_H_ */
