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
 
#include "InterruptHandler.h"

InterruptHandler InterruptHandler::instance;
callback_t InterruptHandler::GPIOA_interruptVector[8];
callback_t InterruptHandler::GPIOB_interruptVector[8];
callback_t InterruptHandler::GPIOC_interruptVector[8];
callback_t InterruptHandler::GPIOD_interruptVector[8];

InterruptHandler::InterruptHandler()
{
    GPIOPortIntRegister(GPIO_A_BASE, GPIOA_InterruptHandler);
    GPIOPortIntRegister(GPIO_B_BASE, GPIOB_InterruptHandler);
    GPIOPortIntRegister(GPIO_C_BASE, GPIOC_InterruptHandler);
    GPIOPortIntRegister(GPIO_D_BASE, GPIOD_InterruptHandler);
}

InterruptHandler &InterruptHandler::getInstance(void)
{
    return instance;
}

void InterruptHandler::registerInterruptHandler(uint32_t interrupt_, uint8_t pin_, callback_t callback_)
{
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

    if (interrupt_ == GPIO_A_BASE) {
        GPIOA_interruptVector[pin_] = callback_;
    } else if (interrupt_ == GPIO_B_BASE) {
        GPIOB_interruptVector[pin_] = callback_;
    } else if (interrupt_ == GPIO_C_BASE) {
        GPIOC_interruptVector[pin_] = callback_;
    } else if (interrupt_ == GPIO_D_BASE) {
        GPIOD_interruptVector[pin_] = callback_;
    }

}

void InterruptHandler::GPIOA_InterruptHandler(void)
{
    uint32_t status = GPIOPinIntStatus(GPIO_A_BASE, true);
    
    GPIOPinIntClear(GPIO_A_BASE, status);
    
    if (status & GPIO_PIN_0) {
        GPIOA_interruptVector[0]();
    } else if (status & GPIO_PIN_1) {
        GPIOA_interruptVector[1]();
    } else if (status & GPIO_PIN_2) {
        GPIOA_interruptVector[2]();    
    } else if (status & GPIO_PIN_3) {
        GPIOA_interruptVector[3]();    
    } else if (status & GPIO_PIN_4) {
        GPIOA_interruptVector[4]();    
    } else if (status & GPIO_PIN_5) {
        GPIOA_interruptVector[5]();    
    } else if (status & GPIO_PIN_6) {
        GPIOA_interruptVector[6]();    
    } else if (status & GPIO_PIN_7) {
        GPIOA_interruptVector[7]();    
    }
}

void InterruptHandler::GPIOB_InterruptHandler(void)
{
    uint32_t status = GPIOPinIntStatus(GPIO_B_BASE, true);
    
    GPIOPinIntClear(GPIO_B_BASE, status);
    
    if (status & GPIO_PIN_0) {
        GPIOB_interruptVector[0]();
    } else if (status & GPIO_PIN_1) {
        GPIOB_interruptVector[1]();
    } else if (status & GPIO_PIN_2) {
        GPIOB_interruptVector[2]();    
    } else if (status & GPIO_PIN_3) {
        GPIOB_interruptVector[3]();    
    } else if (status & GPIO_PIN_4) {
        GPIOB_interruptVector[4]();    
    } else if (status & GPIO_PIN_5) {
        GPIOB_interruptVector[5]();    
    } else if (status & GPIO_PIN_6) {
        GPIOB_interruptVector[6]();    
    } else if (status & GPIO_PIN_7) {
        GPIOB_interruptVector[7]();    
    }
}

void InterruptHandler::GPIOC_InterruptHandler(void)
{
    uint32_t status = GPIOPinIntStatus(GPIO_C_BASE, true);
    
    GPIOPinIntClear(GPIO_C_BASE, status);
    
    if (status & GPIO_PIN_0) {
        GPIOC_interruptVector[0]();
    } else if (status & GPIO_PIN_1) {
        GPIOC_interruptVector[1]();
    } else if (status & GPIO_PIN_2) {
        GPIOC_interruptVector[2]();    
    } else if (status & GPIO_PIN_3) {
        GPIOC_interruptVector[3]();    
    } else if (status & GPIO_PIN_4) {
        GPIOC_interruptVector[4]();    
    } else if (status & GPIO_PIN_5) {
        GPIOC_interruptVector[5]();    
    } else if (status & GPIO_PIN_6) {
        GPIOC_interruptVector[6]();    
    } else if (status & GPIO_PIN_7) {
        GPIOC_interruptVector[7]();    
    }
}

void InterruptHandler::GPIOD_InterruptHandler(void)
{
    uint32_t status = GPIOPinIntStatus(GPIO_D_BASE, true);
    
    GPIOPinIntClear(GPIO_D_BASE, status);
    
    if (status & GPIO_PIN_0) {
        GPIOD_interruptVector[0]();
    } else if (status & GPIO_PIN_1) {
        GPIOD_interruptVector[1]();
    } else if (status & GPIO_PIN_2) {
        GPIOD_interruptVector[2]();    
    } else if (status & GPIO_PIN_3) {
        GPIOD_interruptVector[3]();    
    } else if (status & GPIO_PIN_4) {
        GPIOD_interruptVector[4]();    
    } else if (status & GPIO_PIN_5) {
        GPIOD_interruptVector[5]();    
    } else if (status & GPIO_PIN_6) {
        GPIOD_interruptVector[6]();    
    } else if (status & GPIO_PIN_7) {
        GPIOD_interruptVector[7]();    
    }
}
