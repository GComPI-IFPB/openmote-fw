/**
 * @file       Sniffer.h
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

#include "Board.h"
#include "Callback.h"
#include "Radio.h"

#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"

class SnifferCommon;

typedef GenericCallback<SnifferCommon> SnifferCallback;

class SnifferCommon
{
public:
    SnifferCommon(Board& board, Radio& radio);
    virtual void init(void);
    void start(void);
    void stop(void);
    void setChannel(uint8_t channel);
    virtual void processRadioFrame(void) = 0;
protected:
    void radioRxInitCallback(void);
    void radioRxDoneCallback(void);
protected:
    Board board_;
    Radio radio_;

    SnifferCallback snifferRadioRxInitCallback_;
    SnifferCallback snifferRadioRxDoneCallback_;

    xSemaphoreHandle radioRxSemaphore;
    BaseType_t xHigherPriorityTaskWoken;

    uint8_t  radioBuffer[128];
    uint8_t* radioBuffer_ptr;
    uint8_t  radioBuffer_len;

    int8_t  rssi;
    uint8_t lqi;
    uint8_t crc;
};

#endif /* SNIFFER_COMMON_H_ */
