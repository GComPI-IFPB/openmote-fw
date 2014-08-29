/*
 * Copyright 2013 OpenMote Technologies, S.L.
 */

/**
 *
 * @file       Board.h
 * @author     Pere Tuset-Peiro (peretuset@openmote.com)
 * @version    v0.1
 * @date       May, 2014
 * @brief
 * @ingroup
 *
 */

#ifndef BOARD_H_
#define BOARD_H_

#include <stdint.h>

enum SleepMode : uint8_t;

class Board {
public:
    Board();
    void reset(void);
    void setSleepMode(SleepMode sleepMode_);
    void sleep(void);
    void wakeup(void);
    void enableInterrupts(void);
    void disableInterrupts(void);
    void getAddress(uint8_t* address);
private:
    SleepMode sleepMode;
};

#endif /* BOARD_H_ */
