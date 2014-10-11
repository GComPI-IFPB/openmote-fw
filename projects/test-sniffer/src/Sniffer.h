/*
 * Copyright 2013 OpenMote Technologies, S.L.
 */

/**
 *
 * @file       Sniffer.h
 * @author     Pere Tuset-Peiro (peretuset@openmote.com)
 * @version    v0.1
 * @date       May, 2014
 * @brief
 * @ingroup
 *
 */


#ifndef SNIFFER_H_
#define SNIFFER_H_

#include <stdint.h>

#include "Board.h"
#include "Ethernet.h"
#include "Radio.h"

#include "Callback.h"

#include "FreeRTOS.h"
#include "semphr.h"

class Sniffer;

typedef GenericCallback<Sniffer> SnifferCallback;

class Sniffer
{
public:
    Sniffer(Board& board, Ethernet& ethernet, Radio& radio);
    void init(void);
    void start(void);
    void stop(void);
    void setChannel(uint8_t channel);
    void processRadioFrame(void);
private:
    void initEthernetFrame(uint8_t* buffer, uint8_t length, int8_t rssi, uint8_t lqi, uint8_t crc);
private:
    void radioRxInitCallback(void);
    void radioRxDoneCallback(void);
private:
    Board board_;
    Ethernet ethernet_;
    Radio radio_;

    SnifferCallback snifferRadioRxInitCallback_;
    SnifferCallback snifferRadioRxDoneCallback_;

    xSemaphoreHandle radioRxSemaphore;
    BaseType_t xHigherPriorityTaskWoken;

    uint8_t macAddress[6];
    static const uint8_t broadcastAddress[6];
    static const uint8_t ethernetType[2];

    uint8_t  radioBuffer[128];
    uint8_t* radioBuffer_ptr;
    uint8_t  radioBuffer_len;

    uint8_t  ethernetBuffer[255];
    uint8_t* ethernetBuffer_ptr;
    uint32_t  ethernetBuffer_len;

    int8_t  rssi;
    uint8_t lqi;
    uint8_t crc;

};

#endif /* SNIFFER_H_ */
