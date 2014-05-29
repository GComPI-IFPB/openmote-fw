#include "Led.h"

Led::Led()
{
}

Led::Led(uint32_t port_, uint8_t pin_):
    Gpio(port_, pin_)
{
    GPIOPinTypeGPIOOutput(port, pin);
    GPIOPinWrite(port, pin, 0);
}

void Led::on(void)
{
    GPIOPinWrite(port, pin, pin);
}

void Led::off(void)
{
    GPIOPinWrite(port, pin, 0);
}

void Led::toggle(void)
{
    uint32_t status = GPIOPinRead(port, pin);
    
    status = (~status) & pin;
    
    GPIOPinWrite(port, pin, status);
}

