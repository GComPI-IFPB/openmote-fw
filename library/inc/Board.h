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

#include "gpio.h"
#include "ioc.h"
#include "sys_ctrl.h"
#include "interrupt.h"

#include "hw_gpio.h"
#include "hw_ioc.h"
#include "hw_memmap.h"
#include "hw_sys_ctrl.h"
#include "hw_types.h"

class Board {
public:
    Board();
    void init(void);
    void reset(void);
    void sleep(void);
    void enableInterrupts(void);
    void disableInterrupts(void);
private:

};

#endif /* BOARD_H_ */
