/**
 * @file       CircularBuffer.cpp
 * @author     Pere Tuset-Peiro (peretuset@openmote.com)
 * @version    v0.1
 * @date       May, 2015
 * @brief
 *
 * @copyright  Copyright 2015, OpenMote Technologies, S.L.
 *             This file is licensed under the GNU General Public License v2.
 */

/*================================ include ==================================*/

#include "CircularBuffer.h"

#include <string.h>

/*================================ define ===================================*/

/*================================ typedef ==================================*/

/*=============================== variables =================================*/

/*=============================== prototypes ================================*/

/*================================= public ==================================*/

CircularBuffer::CircularBuffer(uint8_t* buffer, uint32_t length):
    Buffer(buffer, length)
{   
}

void CircularBuffer::reset(void)
{
	Buffer::reset();
}

uint32_t CircularBuffer::getSize(void)
{
	return Buffer::getSize();
}

bool CircularBuffer::isEmpty(void)
{
	return Buffer::isEmpty();
}

bool CircularBuffer::isFull(void)
{
	return Buffer::isFull();	
}

bool CircularBuffer::read(uint8_t* data)
{
	return Buffer::read(data);
}

bool CircularBuffer::read(uint8_t* buffer, uint32_t length)
{
	return Buffer::read(buffer, length);
}

bool CircularBuffer::write(uint8_t data)
{
	return Buffer::write(data);
}

bool CircularBuffer::write(const uint8_t* data, uint32_t length)
{
	return Buffer::write(data, length);
}

/*=============================== protected =================================*/

/*================================ private ==================================*/
