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
#include "SerialCallback.h"

static const uint8_t RX_BUFFER_SIZE = 128;
static const uint8_t TX_BUFFER_SIZE = 128;

class Serial
{
public:
    Serial(UartDriver& uart);
    void enable(void);
    void printf(uint8_t* data, uint32_t size);
    void scanf(uint8_t* buffer, uint32_t size);
private:
    void rxCallback_(void);
    void txCallback_(void);
private:
    UartDriver& uart;

    SerialCallback rxCallback;
    SerialCallback txCallback;

    uint8_t rxBuffer[RX_BUFFER_SIZE];
    uint8_t* rxBufferHead;
    uint8_t* rxBufferTail;
    uint32_t rxBufferSize;
    
    uint8_t txBuffer[TX_BUFFER_SIZE];
    uint8_t* txBufferHead;
    uint8_t* txBufferTail;
    uint32_t txBufferSize;
};

#endif /* SERIAL_H_ */
