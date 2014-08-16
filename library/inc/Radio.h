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
    void transmit(void);
    void receive(void);
    void loadPacket(uint8_t* data, uint32_t length);
    void getPacket(uint8_t* buffer, uint32_t length);
    void registerRxInterrupts(Callback* rxInit_, Callback* rxDone_);
    void registerTxInterrupts(Callback* txInit_, Callback* txDone_);
    void enableInterrupts(void);
    void disableInterrupts(void);
    void setChannel(uint8_t channel);
    void setPower(uint8_t power);
protected:
    void interruptHandler(void);
    void errorHandler(void);
private:
    Callback* rxInit;
    Callback* rxDone;
    Callback* txInit;
    Callback* txDone;
};

#endif /* RADIO_H_ */
