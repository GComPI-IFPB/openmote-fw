/**
 * @file       Timer.hpp
 * @author     Pere Tuset-Peiro (peretuset@openmote.com)
 * @version    v0.1
 * @date       May, 2015
 * @brief
 *
 * @copyright  Copyright 2015, OpenMote Technologies, S.L.
 *             This file is licensed under the GNU General Public License v2.
 */

#ifndef TIMER_HPP_
#define TIMER_HPP_

#include <stdint.h>

#include "Callback.hpp"

struct TimerConfig;

class Timer
{

friend class InterruptHandler;

public:
    Timer(TimerConfig& config);
    void init(void);
    void setPrescaler(uint32_t prescaler);
    uint32_t getPrescaler(void);
    uint32_t getFrequency(void);
    void setFrequency(uint32_t frequency);
    void start(void);
    void stop(void);
    uint32_t read(void);
    void setCallback(Callback* callback);
    void clearCallback(void);
    void enableInterrupts(void);
    void disableInterrupts(void);
protected:
    TimerConfig& getConfig(void);
    void interruptHandler(void);
protected:
    TimerConfig& config_;
    
    Callback* callback_;
    
    uint32_t frequency_;
    uint32_t prescaler_;
};

#endif /* TIMER_HPP_ */
