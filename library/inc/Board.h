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

#include "sys_ctrl.h"

/**
 * SleepMode_None:   ~2 mA,   0 + 0 us,   wake-up from any interrupt source (e.g. UART)
 * SleepMode_1:    ~600 uA, 0.5 + 4 us,   wake-up from Gpio, Sleep timer, USB resume
 * SleepMode_2:    ~1.5 uA, 136 + 136 us, wake-up from Gpio, Sleep timer
 * SleepMode_3:    ~0.8 uA, 136 + 136 us, wake-up from Gpio
 */
typedef enum {
    SleepMode_None = SYS_CTRL_PM_NOACTION,
    SleepMode_1    = SYS_CTRL_PM_1,
    SleepMode_3    = SYS_CTRL_PM_2,
    SleepMode_2    = SYS_CTRL_PM_3
} SleepMode;

class Board {
public:
    Board();
    void reset(void);
    void setSleepMode(SleepMode sleepMode_);
    void sleep(void);
    void wakeup(void);
    void enableInterrupts(void);
    void disableInterrupts(void);
private:
    SleepMode sleepMode = SleepMode_None;
};

#endif /* BOARD_H_ */
