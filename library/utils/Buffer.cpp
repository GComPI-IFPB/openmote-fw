/**
 * @file       Buffer.cpp
 * @author     Pere Tuset-Peiro (peretuset@openmote.com)
 * @version    v0.1
 * @date       May, 2015
 * @brief
 *
 * @copyright  Copyright 2015, OpenMote Technologies, S.L.
 *             This file is licensed under the GNU General Public License v2.
 */

/*================================ include ==================================*/

#include "Buffer.h"

#include <string.h>

/*================================ define ===================================*/

/*================================ typedef ==================================*/

/*=============================== variables =================================*/

/*=============================== prototypes ================================*/

/*================================= public ==================================*/

Buffer::Buffer(uint8_t* buffer, uint32_t length):
    buffer_(buffer), length_(length), count_(0), head_(buffer), tail_(buffer)
{   
}

void Buffer::reset(void)
{
    // Try to acquire the mutex
    if (!rmutex_.take())
    {
        while(true);
    }

    // Reset the circular buffer
    head_ = buffer_;
    tail_ = buffer_;
    count_ = 0;

    // Free the mutex
    rmutex_.give();
}

bool Buffer::isEmpty(void)
{
    bool status;

    // Try to acquire the mutex
    if (!rmutex_.take())
    {
        while (true);
    }

    // Get the status
    status = (count_ == 0);

    // Free the mutex
    rmutex_.give();

    return status;
}

bool Buffer::isFull(void)
{
    bool status;

    if (!rmutex_.take())
    {
        while (true);
    }

    // Get the status
    status = (count_ == length_);

    // Free the mutex
    rmutex_.give();

    return status;
}

uint32_t Buffer::getSize(void)
{
    uint32_t count;

    if (!rmutex_.take())
    {
        while (true);
    }

    // Copy the value
    count = count_;

    // Free the mutex
    rmutex_.give();

    return count;
}

bool Buffer::read(uint8_t* data)
{
    // Try to acquire the mutex
    if (!rmutex_.take())
    {
        while (true);
    }

    // Check if buffer is empty
    if (!isEmpty())
    {
        // Read data in and count it
        *data = *tail_++;
        count_ -= 1;

        // Free the mutex
        rmutex_.give();

        // Return success
        return true;
    }
    else
    {
        // Free the mutex
        rmutex_.give();

        // Return error
        return false;
    }
}

bool Buffer::read(uint8_t* buffer, uint32_t length)
{
    bool status = false;

    // For each byte
    while (length--)
    {
        // Try to read the byte
        status = read(buffer++);
        if (!status)
        {
            break;
        }
    }

    // Return status
    return status;
}

bool Buffer::write(uint8_t data)
{
    // Try to acquire the mutex
    if (!rmutex_.take())
    {
        while (true);
    }

    // Check if buffer is full
    if (!isFull())
    {
        // Write data in and count it
        *head_++ = data;
        count_ += 1;

        // Free the mutex
        rmutex_.give();

        // Return success
        return true;
    }
    else
    {
        // Free the mutex
        rmutex_.give();

        // Return error
        return false;
    }
}

bool Buffer::write(const uint8_t *data, uint32_t length)
{
    bool status = false;

    // For each byte
    while (length--)
    {
        // Try to write the byte
        status = write(*data++);
        if (!status)
        {
            break;
        }
    }

    // Return status
    return status;
}

/*=============================== protected =================================*/

/*================================ private ==================================*/
