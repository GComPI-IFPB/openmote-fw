/**
 * @file       GpioPwm.cpp
 * @author     Pere Tuset-Peiro (peretuset@openmote.com)
 * @version    v0.1
 * @date       May, 2015
 * @brief
 *
 * @copyright  Copyright 2015, OpenMote Technologies, S.L.
 *             This file is licensed under the GNU General Public License v2.
 */

/*================================ include ==================================*/

#include <Gpio.hpp>
#include <InterruptHandler.hpp>
#include "cc2538_include.h"
#include "platform_types.h"

/*================================ define ===================================*/

/*================================ typedef ==================================*/

/*=============================== variables =================================*/

/*=============================== prototypes ================================*/

/*================================= public ==================================*/

GpioPwm::GpioPwm(GpioConfig& gpioConfig, TimerConfig& timerConfig):
    GpioOut(gpioConfig), Timer(timerConfig)
{

}

void GpioPwm::init(bool level)
{
    Timer::init();

    TimerControlLevel(Timer::config_.base, Timer::config_.source, level);

    IOCPinConfigPeriphOutput(GpioOut::config_.port, GpioOut::config_.pin, GpioOut::config_.ioc);

    GPIOPinTypeTimer(GpioOut::config_.port, GpioOut::config_.pin);

    IOCPadConfigSet(GpioOut::config_.port, GpioOut::config_.pin, IOC_OVERRIDE_OE);
}

void GpioPwm::setCallback(Callback* callback)
{
	Timer::setCallback(callback);
}

void GpioPwm::clearCallback(void)
{
    Timer::clearCallback();
}

void GpioPwm::enableInterrupts(void)
{
	Timer::enableInterrupts();
}

void GpioPwm::disableInterrupts(void)
{
	Timer::disableInterrupts();
}

void GpioPwm::setFrequency(uint16_t frequency)
{
	Timer::setPrescaler(1);
    Timer::setFrequency(frequency);
}

void GpioPwm::setDutyCycle(uint16_t dutyCycle)
{
	dutyCycle_ = dutyCycle;	

	TimerMatchSet(Timer::config_.base, Timer::config_.source, dutyCycle_);
}

void GpioPwm::start(void)
{
	Timer::start();
}

void GpioPwm::stop(void)
{
	Timer::stop();
}

/*=============================== protected =================================*/

/*================================ private ==================================*/
