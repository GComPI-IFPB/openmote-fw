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

class Board {
public:
    Board();
    void reset(void);
    void sleep(void);
    void setDeepSleepMode(uint32_t deepSleepMode_);
    void deepSleep(void);
    void enableInterrupts(void);
    void disableInterrupts(void);
private:
    void defaultPeripheralSettings(void);
private:
    
};

#endif /* BOARD_H_ */
