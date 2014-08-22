/*
 * Copyright 2013 OpenMote Technologies, S.L.
 */

/**
 *
 * @file       Radio.h
 * @author     Pere Tuset-Peiro (peretuset@openmote.com)
 * @version    v0.1
 * @date       May, 2014
 * @brief
 * @ingroup
 *
 */

#ifndef RADIO_H_
#define RADIO_H_

#include <stdint.h>

#include "Callback.h"

class Radio
{

friend class InterruptHandler;

public:
    Radio();
    void enable(void);
    void sleep(void);
    void wakeup(void);
    void reset(void);
    void transmit(void);
    void receive(void);
    void loadPacket(uint8_t* data, uint8_t length);
    uint8_t getPacket(uint8_t* buffer, uint8_t length, int8_t* rssi, uint8_t* crc);
    void registerRxCallbacks(Callback* rxInit_, Callback* rxDone_);
    void registerTxCallbacks(Callback* txInit_, Callback* txDone_);
    void setChannel(uint8_t channel);
    void setPower(uint8_t power);
protected:
    void interruptHandler(void);
    void errorHandler(void);
private:
    void on(void);
    void off(void);
    void enableInterrupts(void);
    void disableInterrupts(void);
private:
    Callback* rxInit;
    Callback* rxDone;
    Callback* txInit;
    Callback* txDone;
};

#endif /* RADIO_H_ */
