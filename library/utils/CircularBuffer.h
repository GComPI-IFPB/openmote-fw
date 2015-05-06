/*
 * Copyright 2013 OpenMote Technologies, S.L.
 */

/**
 *
 * @file       CircularBuffer.h
 * @author     Pere Tuset-Peiro (peretuset@openmote.com)
 * @version    v0.1
 * @date       May, 2014
 * @brief
 * @ingroup
 *
 */

#ifndef CIRCULAR_BUFFER_H_
#define CIRCULAR_BUFFER_H_

#include <stdint.h>

#include "FreeRTOS.h"
#include "semphr.h"

class CircularBuffer
{
public:
    CircularBuffer(uint8_t* buffer, int32_t length);
    void reset(void);
    uint32_t getSize(void);
    bool isEmpty(void);
    bool isFull(void);
    int32_t read(uint8_t* data);
    int32_t read(uint8_t* buffer, int32_t length);
    int32_t write(uint8_t data);
    int32_t write(const uint8_t* data, int32_t length);
private:
    void empty(void);
private:
    SemaphoreHandle_t mutex_;
    uint8_t* buffer_;
    int32_t  length_;
    int32_t count_;
    uint8_t* head_;
    uint8_t* tail_;
};

#endif /* CIRCULAR_BUFFER_H_ */
