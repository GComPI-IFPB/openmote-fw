/*
 * Copyright 2013 OpenMote Technologies, S.L.
 */

/**
 *
 * @file       Enc28j60.h
 * @author     Pere Tuset-Peiro (peretuset@openmote.com)
 * @version    v0.1
 * @date       May, 2014
 * @brief
 * @ingroup
 *
 */

#include "SpiDriver.h"

class Enc28j60
{
public:
    Enc28j60(SpiDriver spi_);
protected:

private:
    SpiDriver spi;
};
