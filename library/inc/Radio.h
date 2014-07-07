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

#include "interrupt.h"
#include "ioc.h"
#include "sys_ctrl.h"

#include "hw_ints.h"
#include "hw_rfcore_sfr.h"
#include "hw_rfcore_xreg.h"
#include "hw_memmap.h"
#include "hw_types.h"

typedef void (*callback_t)(void);

class Radio
{

friend class InterruptHandler;

public:
    Radio & getInstance(void);
    void enable(void);
    void sleep(void);
    void wakeup(void);
    void transmit(void);
    void receive(void);
    void loadPacket(void);
    void getPacket(void);
    void registerRxInterrupts(callback_t rx_init_, callback_t rx_done_);
    void registerTxInterrupts(callback_t tx_init_, callback_t tx_done_);
    void enableInterrupts(void);
    void disableInterrupts(void);
    void setChannel(uint8_t channel);
    void setPower(uint8_t power);
protected:
    void interruptHandler(void);
    void errorHandler(void);
private:
    Radio();
    void flushRxBuffer(void);
    void flushTxBuffer(void);
private:
    static Radio instance;
    static callback_t rx_init, rx_done;
    static callback_t tx_init, tx_done;
};

#endif /* RADIO_H_ */
