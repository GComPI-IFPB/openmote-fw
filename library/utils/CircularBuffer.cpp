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

CircularBuffer::CircularBuffer(uint8_t* buffer, int32_t length):
    buffer_(buffer), length_(length), count_(0), head_(buffer), tail_(buffer)
{
    mutex_ = xSemaphoreCreateMutex();
    if (mutex_ == NULL) {
        while(true);
    }
}

void CircularBuffer::reset(void)
{
    // Try to acquire the mutex
    if (xSemaphoreTake(mutex_, portMAX_DELAY) == pdTRUE)
    {
        empty();
        
        head_ = buffer_;
        tail_ = buffer_;
        count_ = 0;
        
        xSemaphoreGive(mutex_);
    }
}

bool CircularBuffer::isEmpty(void)
{
    return (length_ == 0);
}

bool CircularBuffer::isFull(void)
{
    return (count_ == length_);
}

uint32_t CircularBuffer::getSize(void)
{
    return count_;
}

int32_t CircularBuffer::read(uint8_t* data)
{
    // Try to acquire the mutex
    if (xSemaphoreTake(mutex_, portMAX_DELAY) != pdTRUE)
    {
        return -1;
    }
    
    // Check if buffer is empty
    if (!isEmpty())
    {
        // Read data in and count it
        *data = *tail_++;
        count_--;

        // Check for tail pointer wrap around
        if (tail_ == (buffer_ + length_))
        {
            tail_ = buffer_;
        }

        // Free the mutex
        xSemaphoreGive(mutex_);

        // Return success
        return 0;
    }
    else
    {
        // Free the mutex
        xSemaphoreGive(mutex_);
        
        // Return error
        return -1;
    }
}

int32_t CircularBuffer::read(uint8_t* buffer, int32_t length)
{
    // For each byte
    while (length--)
    {
        // Try to read the byte
        if (read(buffer++) != 0)
        {
            // Return error
            return -1;
        }
    }

    // Return success
    return 0;
}

int32_t CircularBuffer::write(uint8_t data)
{
    // Try to acquire the mutex
    if (xSemaphoreTake(mutex_, portMAX_DELAY) != pdTRUE)
    {
        return -1;
    }

    // Check if buffer is full
    if (!isFull())
    {
        // Write data in and count it
        *head_++ = data;
        count_ += 1;

        // Check for head pointer wrap around
        if (head_ == (buffer_ + length_))
        {
            head_ = buffer_;
        }

        // Free the mutex
        xSemaphoreGive(mutex_);

        // Return success
        return 0;
    }
    else
    {
        // Free the mutex
        xSemaphoreGive(mutex_);
        
        // Return error
        return -1;
    }
}

int32_t CircularBuffer::write(const uint8_t *data, int32_t length)
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

/*=============================== protected =================================*/

/*================================ private ==================================*/

void CircularBuffer::empty(void)
{
    uint8_t* tmp = buffer_;
    while(tmp != (buffer_ + length_))
    {
        *tmp++ = 0;
    }
}
