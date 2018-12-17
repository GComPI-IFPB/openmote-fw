/**
 * @file       SysTick.hpp
 * @author     Pere Tuset-Peiro (peretuset@openmote.com)
 * @version    v0.1
 * @date       May, 2015
 * @brief
 *
 * @copyright  Copyright 2015, OpenMote Technologies, S.L.
 *             This file is licensed under the GNU General Public License v2.
 */

#ifndef SYSTICK_HPP_
#define SYSTICK_HPP_

#include <stdint.h>

#include "Callback.hpp"

class SysTick
{

friend class InterruptHandler;

public:
    SysTick(uint32_t period);
    void init(void);
    void start(void);
    void stop(void);
    void setCallback(Callback* callback);
    void clearCallback(void);
    void enableInterrupts(void);
    void disableInterrupts(void);
protected:
    void interruptHandler(void);
private:
    uint32_t period_;

    Callback* callback_;
};

#endif /* SYSTICK_HPP_ */
