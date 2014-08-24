/*
 * Copyright 2013 OpenMote Technologies, S.L.
 */

/**
 *
 * @file       Hdlc.h
 * @author     Pere Tuset-Peiro (peretuset@openmote.com)
 * @version    v0.1
 * @date       May, 2014
 * @brief
 * @ingroup
 *
 */

#ifndef HDLC_H_
#define HDLC_H_

#include <stdint.h>

class Hdlc
{
public:
    Hdlc();

    void rxOpen(uint8_t* buffer, uint8_t length);
    void rxPut(uint8_t byte);
    void rxClose(void);

    void txOpen(uint8_t* buffer, uint8_t length);
    void txPut(uint8_t byte);
    void txClose(void);

private:
    void rxParse(uint8_t byte);

private:
    uint8_t lastByte;
    bool isEscaping;
};

#endif /* HDLC_H_ */
