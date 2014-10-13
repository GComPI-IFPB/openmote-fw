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

#include "Callback.h"

enum SleepMode : uint8_t;

class Board;

typedef GenericCallback<Board> BoardCallback;

class Board {
public:
    Board();
    void reset(void);
    void setSleepMode(SleepMode sleepMode_);
    void sleep(void);
    void wakeup(void);
    void enableInterrupts(void);
    void disableInterrupts(void);
    void enableFlashErase(void);
    void getEUI48(uint8_t* address);
    void getEUI64(uint8_t* address);
private:
    void flashEraseCallback_(void);
private:
    SleepMode sleepMode;
    BoardCallback flashEraseCallback;
};

#endif /* BOARD_H_ */
