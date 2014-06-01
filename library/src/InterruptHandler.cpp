/*
 * Copyright 2013 OpenMote Technologies, S.L.
 */

/**
 *
 * @file       InterruptHandler.cpp
 * @author     Pere Tuset-Peiro (peretuset@openmote.com)
 * @version    v0.1
 * @date       May, 2014
 * @brief
 * @ingroup
 *
 */
 
#include "Gpio.h"
#include "InterruptHandler.h"
#include "Uart.h"

InterruptHandler InterruptHandler::instance;

Gpio* InterruptHandler::GPIOA_interruptVector[8];
Gpio* InterruptHandler::GPIOB_interruptVector[8];
Gpio* InterruptHandler::GPIOC_interruptVector[8];
Gpio* InterruptHandler::GPIOD_interruptVector[8];

Uart* InterruptHandler::UART0_interruptVector;
Uart* InterruptHandler::UART1_interruptVector;

InterruptHandler::InterruptHandler()
{
    GPIOPortIntRegister(GPIO_A_BASE, GPIOA_InterruptHandler);
    GPIOPortIntRegister(GPIO_B_BASE, GPIOB_InterruptHandler);
    GPIOPortIntRegister(GPIO_C_BASE, GPIOC_InterruptHandler);
    GPIOPortIntRegister(GPIO_D_BASE, GPIOD_InterruptHandler);
    
    UARTIntRegister(UART0_BASE, UART0_InterruptHandler);
    UARTIntRegister(UART1_BASE, UART1_InterruptHandler);
}

InterruptHandler &InterruptHandler::getInstance(void)
{
    return instance;
}

void InterruptHandler::registerInterruptHandler(Gpio * gpio_)
{

    uint32_t port_ = gpio_->getPort();
    uint8_t pin_   = gpio_->getPin();

    if (pin_ == GPIO_PIN_0) {
        pin_ = 0;
    } else if (pin_ == GPIO_PIN_1) {
        pin_ = 1;
    } else if (pin_ == GPIO_PIN_2) {
        pin_ = 2;
    } else if (pin_ == GPIO_PIN_3) {
        pin_ = 3;
    } else if (pin_ == GPIO_PIN_4) {
        pin_ = 4;
    } else if (pin_ == GPIO_PIN_5) {
        pin_ = 5;
    } else if (pin_ == GPIO_PIN_6) {
        pin_ = 6;
    } else if (pin_ == GPIO_PIN_7) {
        pin_ = 7;
    }

    if (port_ == GPIO_A_BASE) {
        GPIOA_interruptVector[pin_] = gpio_;
    } else if (port_ == GPIO_B_BASE) {
        GPIOB_interruptVector[pin_] = gpio_;
    } else if (port_ == GPIO_C_BASE) {
        GPIOC_interruptVector[pin_] = gpio_;
    } else if (port_ == GPIO_D_BASE) {
        GPIOD_interruptVector[pin_] = gpio_;
    }   
}

void InterruptHandler::registerInterruptHandler(Uart * uart_)
{
    uint32_t port_ = uart_->getPort();

    if (port_ == UART0_BASE) {
        UART0_interruptVector = uart_;
    } else if (port_ == UART1_BASE) {
        UART1_interruptVector = uart_;
    }
}

inline void InterruptHandler::GPIOA_InterruptHandler(void)
{
    uint32_t status = GPIOPinIntStatus(GPIO_A_BASE, true);
    
    GPIOPinIntClear(GPIO_A_BASE, status);
    
    if (status & GPIO_PIN_0) {
        GPIOA_interruptVector[0]->interruptHandler();
    } else if (status & GPIO_PIN_1) {
        GPIOA_interruptVector[1]->interruptHandler();
    } else if (status & GPIO_PIN_2) {
        GPIOA_interruptVector[2]->interruptHandler();    
    } else if (status & GPIO_PIN_3) {
        GPIOA_interruptVector[3]->interruptHandler();    
    } else if (status & GPIO_PIN_4) {
        GPIOA_interruptVector[4]->interruptHandler();    
    } else if (status & GPIO_PIN_5) {
        GPIOA_interruptVector[5]->interruptHandler();    
    } else if (status & GPIO_PIN_6) {
        GPIOA_interruptVector[6]->interruptHandler();    
    } else if (status & GPIO_PIN_7) {
        GPIOA_interruptVector[7]->interruptHandler();
    }
}

inline void InterruptHandler::GPIOB_InterruptHandler(void)
{
    uint32_t status = GPIOPinIntStatus(GPIO_B_BASE, true);
    
    GPIOPinIntClear(GPIO_B_BASE, status);
    
    if (status & GPIO_PIN_0) {
        GPIOB_interruptVector[0]->interruptHandler();
    } else if (status & GPIO_PIN_1) {
        GPIOB_interruptVector[1]->interruptHandler();
    } else if (status & GPIO_PIN_2) {
        GPIOB_interruptVector[2]->interruptHandler();
    } else if (status & GPIO_PIN_3) {
        GPIOB_interruptVector[3]->interruptHandler();
    } else if (status & GPIO_PIN_4) {
        GPIOB_interruptVector[4]->interruptHandler();
    } else if (status & GPIO_PIN_5) {
        GPIOB_interruptVector[5]->interruptHandler();
    } else if (status & GPIO_PIN_6) {
        GPIOB_interruptVector[6]->interruptHandler();
    } else if (status & GPIO_PIN_7) {
        GPIOB_interruptVector[7]->interruptHandler();
    }
}

inline void InterruptHandler::GPIOC_InterruptHandler(void)
{
    uint32_t status = GPIOPinIntStatus(GPIO_C_BASE, true);
    
    GPIOPinIntClear(GPIO_C_BASE, status);
    
    if (status & GPIO_PIN_0) {
        GPIOC_interruptVector[0]->interruptHandler();
    } else if (status & GPIO_PIN_1) {
        GPIOC_interruptVector[1]->interruptHandler();
    } else if (status & GPIO_PIN_2) {
        GPIOC_interruptVector[2]->interruptHandler();
    } else if (status & GPIO_PIN_3) {
        GPIOC_interruptVector[3]->interruptHandler();
    } else if (status & GPIO_PIN_4) {
        GPIOC_interruptVector[4]->interruptHandler();
    } else if (status & GPIO_PIN_5) {
        GPIOC_interruptVector[5]->interruptHandler();
    } else if (status & GPIO_PIN_6) {
        GPIOC_interruptVector[6]->interruptHandler();
    } else if (status & GPIO_PIN_7) {
        GPIOC_interruptVector[7]->interruptHandler();
    }
}

inline void InterruptHandler::GPIOD_InterruptHandler(void)
{
    uint32_t status = GPIOPinIntStatus(GPIO_D_BASE, true);
    
    GPIOPinIntClear(GPIO_D_BASE, status);
    
    if (status & GPIO_PIN_0) {
        GPIOD_interruptVector[0]->interruptHandler();
    } else if (status & GPIO_PIN_1) {
        GPIOD_interruptVector[1]->interruptHandler();
    } else if (status & GPIO_PIN_2) {
        GPIOD_interruptVector[2]->interruptHandler();
    } else if (status & GPIO_PIN_3) {
        GPIOD_interruptVector[3]->interruptHandler();
    } else if (status & GPIO_PIN_4) {
        GPIOD_interruptVector[4]->interruptHandler();
    } else if (status & GPIO_PIN_5) {
        GPIOD_interruptVector[5]->interruptHandler();
    } else if (status & GPIO_PIN_6) {
        GPIOD_interruptVector[6]->interruptHandler();
    } else if (status & GPIO_PIN_7) {
        GPIOD_interruptVector[7]->interruptHandler();
    }
}

inline void InterruptHandler::UART0_InterruptHandler(void)
{
    UART0_interruptVector->interruptHandler();
}

inline void InterruptHandler::UART1_InterruptHandler(void)
{
    UART1_interruptVector->interruptHandler();
}
