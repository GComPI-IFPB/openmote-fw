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
    Gpio(uint32_t port_, uint8_t pin_):
        port(port_), pin(pin_) {}
    uint32_t getPort(void) {return port;}
    uint8_t getPin(void) {return pin;}
protected:
    uint32_t port;
    uint8_t pin;
};

class GpioI2c : public Gpio
{
public:
    GpioI2c(uint32_t port_, uint8_t pin_):
        Gpio(port_, pin_){}
};

class GpioSpi : public Gpio
{
public:
    GpioSpi(uint32_t port_, uint8_t pin_, uint32_t ioc_):
        Gpio(port_, pin_){}
    uint32_t getIoc(void) {return ioc;}
private:
    uint32_t ioc;
};

class GpioUart : public Gpio
{
public:
    GpioUart(uint32_t port_, uint8_t pin_, uint32_t ioc_):
        Gpio(port_, pin_), ioc(ioc_){}
    uint32_t getIoc(void) {return ioc;}
private:
    uint32_t ioc;
};

#endif /* GPIO_H_ */
