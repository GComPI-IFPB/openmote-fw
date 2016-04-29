/**
 * @file       Timer.h
 * @author     Pere Tuset-Peiro (peretuset@openmote.com)
 * @version    v0.1
 * @date       May, 2015
 * @brief
 *
 * @copyright  Copyright 2015, OpenMote Technologies, S.L.
 *             This file is licensed under the GNU General Public License v2.
 */

#ifndef TIMER_H_
#define TIMER_H_

#include <stdint.h>

#include "Callback.h"

struct TimerConfig;

class Timer
{

friend class InterruptHandler;

public:
    Timer(TimerConfig& config);
    uint32_t getBase(void);
    uint32_t getSource(void);
    void init(uint32_t frequency);
    void start(void);
    void stop(void);
    uint32_t read(void);
    void setCallback(Callback* callback);
    void clearCallback(void);
    void enableInterrupts(void);
    void disableInterrupts(void);
protected:
    void interruptHandler(void);
private:
    TimerConfig& config_;

    Callback* callback_;
};

#endif /* TIMER_H_ */
