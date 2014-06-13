#include "Led.h"

Led::Led(uint32_t port_, uint8_t pin_):
    Gpio(port_, pin_)
{
    GPIOPinTypeGPIOOutput(port, pin);
    GPIOPinWrite(port, pin, 0);
}
