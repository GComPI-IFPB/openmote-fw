#include "Gpio.h"

Gpio::Gpio()
{
}

Gpio::Gpio(uint32_t port_, uint8_t pin_)
{
    port = port_;
    pin = pin_;
}

Gpio::~Gpio()
{
}

void Gpio::gpioRegisterCallback(callback_t callback_)
{
}

void Gpio::gpioEnableInterrupt(void)
{
}

void Gpio::gpioDisableInterrupt(void)
{
}
