#ifndef BUTTON_H_
#define BUTTON_H_

#include "Gpio.h"

class Button : public Gpio
{

public:
    Button();
    Button(uint32_t port_, uint8_t pin_);
    ~Button();
private:

};

#endif /* BUTTON_H_ */
