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

class CircularBuffer
{
public:
    CircularBuffer(uint8_t* buffer_, uint32_t length_);
    int8_t write(uint8_t* data);
    int8_t write(uint8_t* data, uint32_t length);
    int8_t read(uint8_t* data);
    int8_t read(uint8_t* buffer, uint32_t length);
private:
    bool isEmpty();
    bool isFull();
private:
    uint8_t* buffer;
    uint32_t length;
    uint32_t count;
    uint8_t* head;
    uint8_t* tail;

};

#endif /* CIRCULAR_BUFFER_H_ */
