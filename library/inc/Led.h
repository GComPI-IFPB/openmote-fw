#include "Gpio.h"

class Led : public Gpio
{

public:
    Led();
    Led(uint32_t port_, uint8_t pin_);
    void on(void);
    void off(void);
    void toggle(void);
private:

};
