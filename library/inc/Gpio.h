/*
 * Copyright 2013 OpenMote Technologies, S.L.
 */

/**
 *
 * @file       Gpio.h
 * @author     Pere Tuset-Peiro (peretuset@openmote.com)
 * @version    v0.1
 * @date       May, 2014
 * @brief
 * @ingroup
 *
 */

#ifndef GPIO_H_
#define GPIO_H_

#include <stdint.h>

class Gpio
{
public:
    Gpio(uint32_t port, uint8_t pin);
    uint32_t getPort(void);
    uint8_t getPin(void);
protected:
    uint32_t port_;
    uint8_t pin_;
};

class GpioI2c : public Gpio
{
public:
    GpioI2c(uint32_t port, uint8_t pin);
};

class GpioSpi : public Gpio
{
public:
    GpioSpi(uint32_t port, uint8_t pin, uint32_t ioc);
    uint32_t getIoc(void);
    void high(void);
    void low(void);
private:
    uint32_t ioc_;
};

class GpioUart : public Gpio
{
public:
    GpioUart(uint32_t port, uint8_t pin, uint32_t ioc);
    uint32_t getIoc(void);
private:
    uint32_t ioc_;
};

#endif /* GPIO_H_ */
