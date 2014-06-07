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
#include "uart.h"
#include "i2c.h"

#include "hw_memmap.h"
#include "hw_types.h"

class Gpio;
class Uart;
class I2c;

class InterruptHandler {

public:
    static InterruptHandler& getInstance(void);
    static void registerInterruptHandler(Gpio* gpio_);
    static void registerInterruptHandler(Uart* uart_);
    static void registerInterruptHandler(I2c* i2c_);
    static inline void GPIOA_InterruptHandler(void);
    static inline void GPIOB_InterruptHandler(void);
    static inline void GPIOC_InterruptHandler(void);
    static inline void GPIOD_InterruptHandler(void);
    static inline void UART0_InterruptHandler(void);
    static inline void UART1_InterruptHandler(void);
    static inline void I2C_InterruptHandler(void);
private:
    InterruptHandler();
    static InterruptHandler instance;
    static Gpio* GPIOA_interruptVector[8];
    static Gpio* GPIOB_interruptVector[8];
    static Gpio* GPIOC_interruptVector[8];
    static Gpio* GPIOD_interruptVector[8];
    static Uart* UART0_interruptVector;
    static Uart* UART1_interruptVector;
    static I2c*  I2c_interruptVector;
};

#endif /* INTERRUPT_HANDLER_H */
