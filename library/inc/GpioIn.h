/*
 * Copyright 2013 OpenMote Technologies, S.L.
 */

/**
 *
 * @file       GpioIn.h
 * @author     Pere Tuset-Peiro (peretuset@openmote.com)
 * @version    v0.1
 * @date       May, 2014
 * @brief
 * @ingroup
 *
 */

#ifndef GPIO_IN_H_
#define GPIO_IN_H_

#include <stdint.h>

#include "Gpio.h"
#include "Callback.h"

class GpioIn : public Gpio
{

friend class InterruptHandler;

public:
    GpioIn(uint32_t port, uint8_t pin, uint32_t edge);
    bool read(void);
    void setCallback(Callback* callback);
    void clearCallback(void);
    void enableInterrupts(void);
    void disableInterrupts(void);
protected:
    void interruptHandler(void);
protected:
    uint32_t edge_;

    Callback* callback_;
};

#endif /* GPIO_IN_H_ */
