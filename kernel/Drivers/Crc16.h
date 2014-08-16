/*
 * Copyright 2013 OpenMote Technologies, S.L.
 */

/**
 *
 * @file       Crc16.h
 * @author     Pere Tuset-Peiro (peretuset@openmote.com)
 * @version    v0.1
 * @date       May, 2014
 * @brief
 * @ingroup
 *
 */

#ifndef CRC16_H_
#define CRC16_H_

#include <stdint.h>

class Crc16
{
public:
    Crc16();
    void push(uint8_t byte);
    void set(uint16_t crc16_);
    uint16_t get(void);
private:
    uint16_t crc16;
    static const uint16_t lut[256];
};

#endif /* CRC16_H_ */
