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

#include "hw_types.h"

class Gpio;
class Uart;

class InterruptHandler {

public:
    static InterruptHandler& getInstance(void);
    static void registerGpioInterruptHandler(Gpio* gpio_);
    static void registerUartInterruptHandler(Gpio* tx_, Gpio* rx_);
    static void GPIOA_InterruptHandler(void);
    static void GPIOB_InterruptHandler(void);
    static void GPIOC_InterruptHandler(void);
    static void GPIOD_InterruptHandler(void);
    static void UART0_InterruptHandler(void);
    static void UART1_InterruptHandler(void);
private:
    InterruptHandler();
    static InterruptHandler instance;
    static Gpio* GPIOA_interruptVector[8];
    static Gpio* GPIOB_interruptVector[8];
    static Gpio* GPIOC_interruptVector[8];
    static Gpio* GPIOD_interruptVector[8];
    static Uart* UART0_interruptVector[2];
    static Uart* UART1_interruptVector[2];
};

#endif /* INTERRUPT_HANDLER_H */
