/**
 * @file       Serial.hpp
 * @author     Pere Tuset-Peiro (peretuset@openmote.com)
 * @version    v0.1
 * @date       May, 2015
 * @brief
 *
 * @copyright  Copyright 2015, OpenMote Technologies, S.L.
 *             This file is licensed under the GNU General Public License v2.
 */

#ifndef SERIAL_HPP_
#define SERIAL_HPP_

#include <stdint.h>
#include <stdbool.h>

#include "Uart.hpp"

#include "CircularBuffer.hpp"
#include "Hdlc.hpp"

class Serial;

typedef GenericCallback<Serial> SerialCallback;

class Serial
{
public:
    Serial(Uart& uart);
    void init(void);
    void write(uint8_t* data, uint32_t size);
    uint32_t read(uint8_t* buffer, uint32_t size);
public:
    bool operator==(const Serial& other);
private:
    void rxCallback(void);
    void txCallback(void);
private:
    Uart& uart_;

    uint8_t receive_buffer_[256];
    CircularBuffer rxBuffer_;

    uint8_t transmit_buffer_[256];
    CircularBuffer txBuffer_;

    Hdlc hdlc_;

    SerialCallback rxCallback_;
    SerialCallback txCallback_;
};

#endif /* SERIAL_HPP_ */