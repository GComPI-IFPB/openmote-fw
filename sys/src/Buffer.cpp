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

#include <string.h>

#include "CriticalSection.hpp"
#include "Buffer.hpp"

#include "BoardImplementation.hpp"

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
  CriticalSection cs;

  /* Reset the circular buffer */
  head_ = buffer_;
  tail_ = buffer_;
  count_ = 0;
}

bool Buffer::isEmpty(void)
{
  bool status;

  /* Get the status */
  status = (count_ == 0);

  return status;
}

bool Buffer::isFull(void)
{
  bool status;

  /* Get the status */
  status = (count_ == length_);

  return status;
}

uint8_t* Buffer::getHead(void)
{
  return buffer_;
}

uint32_t Buffer::getCapacity(void)
{
  return length_;
}

uint32_t Buffer::getSize(void)
{
  uint32_t count;

  /* Copy the value */
  count = count_;

  return count;
}

bool Buffer::readByte(uint8_t* data, bool interrupt)
{
  /* Check if buffer is empty */
  if (!isEmpty())
  {
    /* Read data in and count it */
    *data = *tail_++;
    count_ -= 1;

    /* Return success */
    return true;
  }
  else
  {
    /* Return error */
    return false;
  }
}

bool Buffer::readBytes(uint8_t* buffer, uint32_t length, bool interrupt)
{
  bool status = false;

  /* For each byte */
  while (length--)
  {
    /* Try to read the byte */
    status = readByte(buffer++, interrupt);
    if (!status)
    {
      break;
    }
  }

  /* Return status */
  return status;
}

bool Buffer::writeByte(uint8_t data, bool interrupt)
{
  /* Check if buffer is full */
  if (!isFull())
  {
    /* Write data in and count it */
    *head_++ = data;
    count_ += 1;

    /* Return success */
    return true;
  }
  else
  {
    /* Return error */
    return false;
  }
}

bool Buffer::writeBytes(const uint8_t *data, uint32_t length, bool interrupt)
{
  bool status = false;

  /* For each byte */
  while (length--)
  {
    /* Try to write the byte */
    status = writeByte(*data++, interrupt);
    if (!status)
    {
      break;
    }
  }

  /* Return status */
  return status;
}

/*=============================== protected =================================*/

/*================================ private ==================================*/
