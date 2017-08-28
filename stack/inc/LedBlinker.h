/**
 * @file       LedBlinker.h
 * @author     Pere Tuset-Peiro (peretuset@openmote.com)
 * @version    v0.1
 * @date       May, 2016
 * @brief
 *
 * @copyright  Copyright 2016, OpenMote Technologies, S.L.
 *             This file is licensed under the GNU General Public License v2.
 */

#ifndef LED_BLINKER_
#define LED_BLINKER_

#include "Task.h"

class GpioOut;

class LedBlinker : protected Task
{
public:
    LedBlinker(const char* const name, uint16_t size, uint8_t priority, GpioOut& gpio);
    void setTime(uint32_t timeOn, uint32_t period);
protected:
    void run(void);
private:
    GpioOut& gpio_;
    uint32_t timeOn_, period_;
};

#endif /* LED_BLINKER_ */
