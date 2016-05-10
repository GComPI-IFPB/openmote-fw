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

class GpioIn;
class Spi;

class Cc1200 : public Radio
{
public:
    Cc1200(Spi& spi, GpioIn& gpio0, GpioIn& gpio2, GpioIn& gpio3);
    void enable(void);
    void sleep(void);
    void wakeup(void);
    void on(void);
    void off(void);
    void reset(void);
    void setRxCallbacks(Callback* rxInit, Callback* rxDone);
    void setTxCallbacks(Callback* txInit, Callback* txDone);
    void enableInterrupts(void);
    void disableInterrupts(void);
    void setChannel(uint8_t channel);
    void setPower(uint8_t power);
    void transmit(void);
    void receive(void);
    RadioResult loadPacket(uint8_t* data, uint8_t length);
    RadioResult getPacket(uint8_t* buffer, uint8_t* length, int8_t* rssi, uint8_t* lqi, uint8_t* crc);
private:
	void strobe(uint8_t strobe);
	uint8_t state(void);
private:
    Spi& spi_;
    GpioIn& gpio0_;
    GpioIn& gpio2_;
    GpioIn& gpio3_;
};

#endif /* CC1200_H_ */
