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

class Serial;

class SerialCallback : public Callback
{
public:
    SerialCallback(Serial* object_ = nullptr, \
                   void(Serial:: *method_)(void) = nullptr):
                   object(object_), method(method_){}
    void execute(void) {(object->*method)();}
private:
    Serial* object;
    void(Serial:: *method)(void);
};

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

    uint8_t rxBuffer[128];
    uint8_t* rxBufferHead;
    uint8_t* rxBufferTail;
    uint32_t rxBufferSize;

    uint8_t txBuffer[128];
    uint8_t* txBufferHead;
    uint8_t* txBufferTail;
    uint32_t txBufferSize;
};

#endif /* SERIAL_H_ */
