/**
 * @file       Gpio.h
 * @author     Pere Tuset-Peiro (peretuset@openmote.com)
 * @version    v0.1
 * @date       May, 2015
 * @brief
 *
 * @copyright  Copyright 2015, OpenMote Technologies, S.L.
 *             This file is licensed under the GNU General Public License v2.
 */

#ifndef GPIO_H_
#define GPIO_H_

#include <stdint.h>

#include "Callback.h"

struct GpioConfig;

class Gpio
{
public:
    Gpio(GpioConfig& config);
    GpioConfig& getGpioConfig(void);
public:
    bool operator==(const Gpio& other);
protected:
    GpioConfig& config_;
};

/*****************************************************************************/

class GpioIn : public Gpio
{

friend class InterruptHandler;

public:
    GpioIn(GpioConfig& config);
    bool read(void);
    void setCallback(Callback* callback);
    void clearCallback(void);
    void enableInterrupts(void);
    void disableInterrupts(void);
protected:
    void interruptHandler(void);
protected:
    Callback* callback_;
};

/*****************************************************************************/

class GpioInPow : public GpioIn
{

public:
    GpioInPow(GpioConfig& config);
    void enableInterrupts(void);
    void disableInterrupts(void);
};

/*****************************************************************************/

class GpioOut : public Gpio
{

public:
    GpioOut(GpioConfig& config);
    void on(void);
    void off(void);
    void toggle(void);
    void high(void);
    void low(void);
    uint32_t status(void);
};

/*****************************************************************************/

class GpioAdc : public Gpio
{
public:
    GpioAdc(GpioConfig& config);
    void init(uint32_t resolution, uint32_t reference);
    uint32_t read(void);
};

/*****************************************************************************/

#endif /* GPIO_H_ */
