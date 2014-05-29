#ifndef GPIO_H_
#define GPIO_H_

#include "gpio.h"

typedef void (*callback_t)(void);

class Gpio
{

public:
    Gpio();
    Gpio(uint32_t port_, uint8_t pin_);
    ~Gpio();
    void gpioRegisterCallback(callback_t callback_);
    void gpioEnableInterrupt(void);
    void gpioDisableInterrupt(void);
protected:
    uint32_t port;
    uint8_t pin;
    uint32_t dir;
    uint32_t mode;
    callback_t callback;
};

#endif /* GPIO_H_ */
