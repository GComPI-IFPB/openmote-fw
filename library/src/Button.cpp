#include "Button.h"

Button::Button()
{

}

Button::Button(uint32_t port_, uint8_t pin_):
    Gpio(port_, pin_)
{
    GPIOPinTypeGPIOInput(port, pin);
}
