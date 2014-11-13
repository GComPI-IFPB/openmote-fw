/*
 * Copyright 2013 OpenMote Technologies, S.L.
 */

/**
 *
 * @file       GpioAdc.cpp
 * @author     Pere Tuset-Peiro (peretuset@openmote.com)
 * @version    v0.1
 * @date       May, 2014
 * @brief
 * @ingroup
 *
 */

/*================================ include ==================================*/

#include "GpioAdc.h"

#include "cc2538_include.h"

/*================================ define ===================================*/

/*================================ typedef ==================================*/

/*=============================== variables =================================*/

/*=============================== prototypes ================================*/

/*================================= public ==================================*/

GpioAdc::GpioAdc(uint32_t port, uint8_t pin, uint32_t adc):
    Gpio(port, pin), adc_(adc)
{
}

void GpioAdc::init(uint32_t resolution, uint32_t reference)
{
    // Configure ADC with resolution and reference
    SOCADCSingleConfigure(resolution, reference);
}

uint32_t GpioAdc::read(void)
{
    uint32_t value;

    // Trigger single conversion on internal temp sensor
    SOCADCSingleStart(adc_);

    // Wait until conversion is completed
    while(!SOCADCEndOfCOnversionGet())
    {
    }

    // Get the ADC value and shift it according to resolution
    value = SOCADCDataGet() >> SOCADC_12_BIT_RSHIFT;

    return value;
}

/*=============================== protected =================================*/

/*================================ private ==================================*/
