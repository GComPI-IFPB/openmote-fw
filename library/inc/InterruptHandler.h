/*
 * Copyright 2013 OpenMote Technologies, S.L.
 */

/**
 *
 * @file       InterruptHandler.h
 * @author     Pere Tuset-Peiro (peretuset@openmote.com)
 * @version    v0.1
 * @date       May, 2014
 * @brief
 * @ingroup
 *
 */

#ifndef INTERRUPT_HANDLER_H_
#define INTERRUPT_HANDLER_H_

#include "gpio.h"

#include "hw_types.h"
#include "hw_memmap.h"

typedef void (*callback_t)(void);

class InterruptHandler {

public:
    static InterruptHandler& getInstance(void);
    static void registerInterruptHandler(uint32_t interrupt_, uint8_t pin_, callback_t callback_);
    static void GPIOA_InterruptHandler(void);
    static void GPIOB_InterruptHandler(void);
    static void GPIOC_InterruptHandler(void);
    static void GPIOD_InterruptHandler(void);
private:
    InterruptHandler();
    static InterruptHandler instance;
    static callback_t GPIOA_interruptVector[8];
    static callback_t GPIOB_interruptVector[8];
    static callback_t GPIOC_interruptVector[8];
    static callback_t GPIOD_interruptVector[8];
};

#endif /* INTERRUPT_HANDLER_H */
