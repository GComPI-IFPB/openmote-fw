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

/**********************************include************************************/

#include "GpioAdc.h"

#include "adc.h"

/**********************************defines************************************/

/*********************************variables***********************************/

/**********************************public*************************************/

GpioAdc::GpioAdc(uint32_t port_, uint8_t pin_, uint32_t adc_):
    Gpio(port_, pin_), adc(adc_)
{
}

void GpioAdc::init(uint32_t resolution_, uint32_t reference_)
{
    // Configure ADC with resolution and reference
    SOCADCSingleConfigure(resolution_, reference_);
}

uint32_t GpioAdc::read(void)
{
    uint32_t value;
    
    // Trigger single conversion on internal temp sensor
    SOCADCSingleStart(adc);
    
    // Wait until conversion is completed
    while(!SOCADCEndOfCOnversionGet())
    {
    }
    
    // Get the ADC value and shift it according to resolution
    value = SOCADCDataGet() >> SOCADC_12_BIT_RSHIFT;
    
    return value;
}

/*********************************protected***********************************/

/**********************************private************************************/

