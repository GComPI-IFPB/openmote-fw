/*
 * Copyright 2013 OpenMote Technologies, S.L.
 */

/**
 *
 * @file       Hdlc.h
 * @author     Pere Tuset-Peiro (peretuset@openmote.com)
 * @version    v0.1
 * @date       May, 2014
 * @brief
 * @ingroup
 *
 */

#ifndef HDLC_H_
#define HDLC_H_

#include <stdint.h>

#include "CircularBuffer.h"
#include "Crc16.h"

enum HdlcStatus : uint8_t
{
    HdlcStatus_Idle = 0x00,
    HdlcStatus_Busy = 0x01,
    HdlcStatus_Done = 0x02,
    HdlcStatus_Error = 0x03,
};

class Hdlc
{
public:
    Hdlc(CircularBuffer& rxCircularBuffer_, CircularBuffer& txCircularBuffer_);

    void rxOpen(void);
    HdlcStatus rxPut(uint8_t byte);
    bool rxClose(void);

    void txOpen(void);
    void txPut(uint8_t byte);
    void txClose(void);

private:
    void rxParse(uint8_t byte);

private:
    CircularBuffer& rxCircularBuffer;
    CircularBuffer& txCircularBuffer;

    HdlcStatus rxStatus;
    uint8_t rxLastByte;
    bool rxIsEscaping;

    Crc16 rxCrc;
    Crc16 txCrc;
};

#endif /* HDLC_H_ */
