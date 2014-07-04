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
#include "I2c.h"

InterruptHandler InterruptHandler::instance;

Gpio* InterruptHandler::GPIOA_interruptVector[8];
Gpio* InterruptHandler::GPIOB_interruptVector[8];
Gpio* InterruptHandler::GPIOC_interruptVector[8];
Gpio* InterruptHandler::GPIOD_interruptVector[8];

Uart* InterruptHandler::UART0_interruptVector;
Uart* InterruptHandler::UART1_interruptVector;

I2c* InterruptHandler::I2c_interruptVector;

InterruptHandler::InterruptHandler()
{
    // Register the GPIO interrupt handlers
    GPIOPortIntRegister(GPIO_A_BASE, GPIOA_InterruptHandler);
    GPIOPortIntRegister(GPIO_B_BASE, GPIOB_InterruptHandler);
    GPIOPortIntRegister(GPIO_C_BASE, GPIOC_InterruptHandler);
    GPIOPortIntRegister(GPIO_D_BASE, GPIOD_InterruptHandler);
    
    // Register the UART interrupt handlers
    UARTIntRegister(UART0_BASE, UART0_InterruptHandler);
    UARTIntRegister(UART1_BASE, UART1_InterruptHandler);
    
    // Register the I2C interrupt handlers
    I2CIntRegister(I2C_InterruptHandler);
}

InterruptHandler &InterruptHandler::getInstance(void)
{
    // Returns an instance of the InterruptHandler
    return instance;
}

void InterruptHandler::registerInterruptHandler(Gpio * gpio_)
{
    // Get the GPIO port and pin
    uint32_t port_ = gpio_->getPort();
    uint8_t pin_   = gpio_->getPin();

    // Select the pin number
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

    // Store a pointer to the GPIO object in the interrupt vector
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
    // Get the UART port
    uint32_t port_ = uart_->getPort();

    // Store a pointer to the UART object in the interrupt vector
    if (port_ == UART0_BASE) {
        UART0_interruptVector = uart_;
    } else if (port_ == UART1_BASE) {
        UART1_interruptVector = uart_;
    }
}

void InterruptHandler::registerInterruptHandler(I2c * i2c_)
{
    // Store a pointer to the I2C object in the interrupt vector
    I2c_interruptVector = i2c_;
}

inline void InterruptHandler::GPIOA_InterruptHandler(void)
{
    uint32_t status;
    
    // Read the GPIO interrupt status (both regular and power)
    uint32_t pin_status = GPIOPinIntStatus(GPIO_A_BASE, true);
    uint32_t pow_status = GPIOPowIntStatus(GPIO_A_BASE, true);
    
    // Clear the regular GPIO interrupt status
    if(pin_status)
    {
        GPIOPinIntClear(GPIO_A_BASE, pin_status);
    }
    
    // Clear the power GPIO interrupt status
    if(pow_status)
    {
        GPIOPowIntClear(GPIO_A_BASE, pow_status);
    }
    
    // Status is both regular and power GPIO interrupt status
    status = pin_status | pow_status;
    
    // Call all the GPIO interrupt handlers
    if (status & GPIO_PIN_0) {
        GPIOA_interruptVector[0]->interruptHandler();
    }
    if (status & GPIO_PIN_1) {
        GPIOA_interruptVector[1]->interruptHandler();
    }
    if (status & GPIO_PIN_2) {
        GPIOA_interruptVector[2]->interruptHandler();    
    }
    if (status & GPIO_PIN_3) {
        GPIOA_interruptVector[3]->interruptHandler();    
    }
    if (status & GPIO_PIN_4) {
        GPIOA_interruptVector[4]->interruptHandler();    
    }
    if (status & GPIO_PIN_5) {
        GPIOA_interruptVector[5]->interruptHandler();    
    }
    if (status & GPIO_PIN_6) {
        GPIOA_interruptVector[6]->interruptHandler();    
    }
    if (status & GPIO_PIN_7) {
        GPIOA_interruptVector[7]->interruptHandler();
    }
}

inline void InterruptHandler::GPIOB_InterruptHandler(void)
{
    uint32_t status;
    
    // Read the GPIO interrupt status (both regular and power)
    uint32_t pin_status = GPIOPinIntStatus(GPIO_B_BASE, true);
    uint32_t pow_status = GPIOPowIntStatus(GPIO_B_BASE, true);
    
    // Clear the regular GPIO interrupt status
    if(pin_status)
    {
        GPIOPinIntClear(GPIO_B_BASE, pin_status);
    }
    
    // Clear the power GPIO interrupt status
    if(pow_status)
    {
        GPIOPowIntClear(GPIO_B_BASE, pow_status);
    }
    
    // Status is both regular and power GPIO interrupt status
    status = pin_status | pow_status;
    
    // Call all the GPIO interrupt handlers
    if (status & GPIO_PIN_0) {
        GPIOB_interruptVector[0]->interruptHandler();
    }
    if (status & GPIO_PIN_1) {
        GPIOB_interruptVector[1]->interruptHandler();
    }
    if (status & GPIO_PIN_2) {
        GPIOB_interruptVector[2]->interruptHandler();
    }
    if (status & GPIO_PIN_3) {
        GPIOB_interruptVector[3]->interruptHandler();
    }
    if (status & GPIO_PIN_4) {
        GPIOB_interruptVector[4]->interruptHandler();
    }
    if (status & GPIO_PIN_5) {
        GPIOB_interruptVector[5]->interruptHandler();
    }
    if (status & GPIO_PIN_6) {
        GPIOB_interruptVector[6]->interruptHandler();
    }
    if (status & GPIO_PIN_7) {
        GPIOB_interruptVector[7]->interruptHandler();
    }
}

inline void InterruptHandler::GPIOC_InterruptHandler(void)
{
    uint32_t status;
    
    // Read the GPIO interrupt status (both regular and power)
    uint32_t pin_status = GPIOPinIntStatus(GPIO_C_BASE, true);
    uint32_t pow_status = GPIOPowIntStatus(GPIO_C_BASE, true);
    
    // Clear the regular GPIO interrupt status
    if(pin_status)
    {
        GPIOPinIntClear(GPIO_C_BASE, pin_status);
    }
    
    // Clear the power GPIO interrupt status
    if(pow_status)
    {
        GPIOPowIntClear(GPIO_C_BASE, pow_status);
    }
    
    // Status is both regular and power GPIO interrupt status
    status = pin_status | pow_status;
    
    // Call all the GPIO interrupt handlers
    if (status & GPIO_PIN_0) {
        GPIOC_interruptVector[0]->interruptHandler();
    }
    if (status & GPIO_PIN_1) {
        GPIOC_interruptVector[1]->interruptHandler();
    }
    if (status & GPIO_PIN_2) {
        GPIOC_interruptVector[2]->interruptHandler();
    }
    if (status & GPIO_PIN_3) {
        GPIOC_interruptVector[3]->interruptHandler();
    }
    if (status & GPIO_PIN_4) {
        GPIOC_interruptVector[4]->interruptHandler();
    }
    if (status & GPIO_PIN_5) {
        GPIOC_interruptVector[5]->interruptHandler();
    }
    if (status & GPIO_PIN_6) {
        GPIOC_interruptVector[6]->interruptHandler();
    }
    if (status & GPIO_PIN_7) {
        GPIOC_interruptVector[7]->interruptHandler();
    }
}

inline void InterruptHandler::GPIOD_InterruptHandler(void)
{
    uint32_t status;
    
    // Read the GPIO interrupt status (both regular and power)
    uint32_t pin_status = GPIOPinIntStatus(GPIO_D_BASE, true);
    uint32_t pow_status = GPIOPowIntStatus(GPIO_D_BASE, true);
    
    // Clear the regular GPIO interrupt status
    if(pin_status)
    {
        GPIOPinIntClear(GPIO_D_BASE, pin_status);
    }
    
    // Clear the power GPIO interrupt status
    if(pow_status)
    {
        GPIOPowIntClear(GPIO_D_BASE, pow_status);
    }
    
    // Status is both regular and power GPIO interrupt status
    status = pin_status | pow_status;
    
    // Call all the GPIO interrupt handlers
    if (status & GPIO_PIN_0) {
        GPIOD_interruptVector[0]->interruptHandler();
    }
    if (status & GPIO_PIN_1) {
        GPIOD_interruptVector[1]->interruptHandler();
    }
    if (status & GPIO_PIN_2) {
        GPIOD_interruptVector[2]->interruptHandler();
    }
    if (status & GPIO_PIN_3) {
        GPIOD_interruptVector[3]->interruptHandler();
    }
    if (status & GPIO_PIN_4) {
        GPIOD_interruptVector[4]->interruptHandler();
    }
    if (status & GPIO_PIN_5) {
        GPIOD_interruptVector[5]->interruptHandler();
    }
    if (status & GPIO_PIN_6) {
        GPIOD_interruptVector[6]->interruptHandler();
    }
    if (status & GPIO_PIN_7) {
        GPIOD_interruptVector[7]->interruptHandler();
    }
}

inline void InterruptHandler::UART0_InterruptHandler(void)
{
    // Call the UART interrupt handler
    UART0_interruptVector->interruptHandler();
}

inline void InterruptHandler::UART1_InterruptHandler(void)
{
    // Call the UART interrupt handler
    UART1_interruptVector->interruptHandler();
}

inline void InterruptHandler::I2C_InterruptHandler(void)
{
    // Call the I2C interrupt handler
    I2c_interruptVector->interruptHandler();
}
