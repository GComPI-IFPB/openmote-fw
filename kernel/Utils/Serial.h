/*
 * Copyright 2013 OpenMote Technologies, S.L.
 */

/**
 *
 * @file       Serial.h
 * @author     Pere Tuset-Peiro (peretuset@openmote.com)
 * @version    v0.1
 * @date       May, 2014
 * @brief
 * @ingroup
 *
 */

#ifndef SERIAL_H_
#define SERIAL_H_

#include <stdint.h>

#include "UartDriver.h"
#include "CircularBuffer.h"
#include "Hdlc.h"

class Serial;

typedef GenericCallback<Serial> SerialCallback;

class Serial
{
public:
    Serial(UartDriver& uart);
    void enable(void);
    void printf(uint8_t* data, uint32_t size);
    uint32_t scanf(uint8_t* buffer, uint32_t size);
private:
    void rxCallback_(void);
    void txCallback_(void);
private:
    UartDriver& uart;

    uint8_t receive_buffer[128];
    CircularBuffer rxBuffer;

    uint8_t transmit_buffer[128];
    CircularBuffer txBuffer;

    Hdlc hdlc;

    SerialCallback rxCallback;
    SerialCallback txCallback;
};

#endif /* SERIAL_H_ */
