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

    uint8_t rxBuffer[];
    uint8_t* rxBuffer_head;
    uint8_t* rxBuffer_tail;
    uint32_t rxBuffer_size;
    
    uint8_t txBuffer[];
    uint8_t* txBuffer_head;
    uint8_t* txBuffer_tail;
    uint32_t txBuffer_size;
};

#endif /* SERIAL_H_ */
