/*
 * Copyright 2013 OpenMote Technologies, S.L.
 */

/**
 *
 * @file       Tps62730.h
 * @author     Pere Tuset-Peiro (peretuset@openmote.com)
 * @version    v0.1
 * @date       May, 2014
 * @brief
 * @ingroup
 *
 */

#ifndef TPS62730_H_
#define TPS62730_H_

#include <stdint.h>

#include "GpioIn.h"
#include "GpioOut.h"

class Tps62730
{

public:
    Tps62730(GpioOut* bypass_, GpioIn* status_);
    void setOn(void);
    void setBypass(void);
    bool getStatus(void);
private:
    GpioOut* bypass;
    GpioIn*  status;
};

#endif /* ADXL346_H_ */
