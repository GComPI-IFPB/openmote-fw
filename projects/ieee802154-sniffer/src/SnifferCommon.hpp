/**
 * @file       SnifferCommon.hpp
 * @author     Pere Tuset-Peiro (peretuset@openmote.com)
 * @version    v0.1
 * @date       May, 2015
 * @brief
 *
 * @copyright  Copyright 2015, OpenMote Technologies, S.L.
 *             This file is licensed under the GNU General Public License v2.
 */

#ifndef SNIFFER_COMMON_H_
#define SNIFFER_COMMON_H_

#include "BoardImplementation.hpp"

#include "Aes.hpp"
#include "Radio.hpp"

#include "Callback.hpp"
#include "Semaphore.hpp"

class SnifferCommon;

typedef GenericCallback<SnifferCommon> SnifferCallback;

class SnifferCommon
{
public:
    SnifferCommon(Board& board, Radio& radio, Aes& aes);
    void init(void);
    void start(void);
    void stop(void);
    void setChannel(uint8_t channel);
    void setKey(bool enable, uint8_t key[16]);
    virtual void processRadioFrame(void) = 0;
    void initFrame(uint8_t* buffer, uint8_t length, int8_t rssi, uint8_t lqi, uint8_t crc);
protected:
    void radioRxInitCallback(void);
    void radioRxDoneCallback(void);
protected:
    Board& board_;
    Radio& radio_;
    Aes& aes_;

    SemaphoreBinary semaphore;

    SnifferCallback snifferRadioRxInitCallback_;
    SnifferCallback snifferRadioRxDoneCallback_;

    uint8_t macAddress[6];
    static const uint8_t broadcastAddress[6];
    static const uint8_t ethernetType[2];

    uint8_t  radioBuffer[128];
    uint8_t* radioBuffer_ptr;
    uint8_t  radioBuffer_len;

    uint8_t  aesBuffer[128];
    uint8_t* aesBuffer_ptr;
    uint8_t  aesBuffer_len;

    uint8_t  outputBuffer[255];
    uint8_t* outputBuffer_ptr;
    uint32_t outputBuffer_len;

    int8_t  rssi;
    uint8_t lqi;
    bool crc;

    bool decrypt;
};

#endif /* SNIFFER_COMMON_H_ */
