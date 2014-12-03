/*
 * Copyright 2013 OpenMote Technologies, S.L.
 */

/**
 *
 * @file       CircularBuffer.cpp
 * @author     Pere Tuset-Peiro (peretuset@openmote.com)
 * @version    v0.1
 * @date       May, 2014
 * @brief
 * @ingroup
 *
 */

/*================================ include ==================================*/

#include "CircularBuffer.h"

/*================================ define ===================================*/

/*================================ typedef ==================================*/

/*=============================== variables =================================*/

/*=============================== prototypes ================================*/

/*================================= public ==================================*/

CircularBuffer::CircularBuffer(uint8_t* buffer_, uint32_t length_):
    buffer(buffer_), length(length_), count(0), head(buffer_), tail(buffer_)
{
}

void CircularBuffer::reset(void)
{
    head = buffer;
    tail = buffer;
    count = 0;
}

uint32_t CircularBuffer::getSize(void)
{
    return count;
}

int8_t CircularBuffer::write(uint8_t data)
{
    // Check if buffer is full
    if (!isFull())
    {
        // Write data in and count it
        *head++ = data;
        count += 1;

        // Check for head pointer wrap around
        if (head == (buffer + length))
        {
            head = buffer;
        }

        // Return success
        return 0;
    }
    else
    {
        // Return error
        return -1;
    }
}

int8_t CircularBuffer::write(uint8_t *data, uint32_t length)
{
    // For each byte
    while (length--)
    {
        // Try to write the byte
        if (write(*data++) != 0)
        {
            // Return error
            return -1;
        }
    }

    // Return success
    return 0;
}

int8_t CircularBuffer::read(uint8_t* data)
{
    // Check if buffer is empty
    if (!isEmpty()){
        // Read data in and count it
        *data = *tail++;
        count--;

        // Check for tail pointer wrap around
        if (tail == (buffer + length))
        {
            tail = buffer;
        }

        // Return success
        return 0;
    }
    else
    {
        // Return error
        return -1;
    }
}

int8_t CircularBuffer::read(uint8_t* buffer, uint32_t length)
{
    // For each byte
    while (length--)
    {
        // Try to write the byte
        if (read(buffer++) != 0)
        {
            // Return error
            return -1;
        }
    }

    // Return success
    return 0;
}

/*=============================== protected =================================*/

/*================================ private ==================================*/

bool CircularBuffer::isEmpty(void)
{
    return (count == 0);
}

bool CircularBuffer::isFull(void)
{
    return (count == length);
}
