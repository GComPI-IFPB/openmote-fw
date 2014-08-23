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

typedef enum
{
    RadioState_Off          = 0x00,
    RadioState_Idle         = 0x01,
    RadioState_ReceiveInit  = 0x02,
    RadioState_Receiving    = 0x03,
    RadioState_ReceiveDone  = 0x04,
    RadioState_TransmitInit = 0x05,
    RadioState_Transmitting = 0x06,
    RadioState_TransmitDone = 0x07
} RadioState;

class Radio
{

friend class InterruptHandler;

public:
    Radio();
    void enable(void);
    void sleep(void);
    void wakeup(void);
    void on(void);
    void off(void);
    void reset(void);
    void setRxCallbacks(Callback* rxInit_, Callback* rxDone_);
    void setTxCallbacks(Callback* txInit_, Callback* txDone_);
    void enableInterrupts(void);
    void disableInterrupts(void);
    void setChannel(uint8_t channel);
    void setPower(uint8_t power);
    void transmit(void);
    void receive(void);
    int8_t loadPacket(uint8_t* data, uint8_t length);
    int8_t getPacket(uint8_t* buffer, uint8_t length, int8_t* rssi, uint8_t* crc);
protected:
    void interruptHandler(void);
    void errorHandler(void);
private:
    volatile RadioState radioState = RadioState_Off;

    Callback* rxInit;
    Callback* rxDone;
    Callback* txInit;
    Callback* txDone;
};

#endif /* RADIO_H_ */
