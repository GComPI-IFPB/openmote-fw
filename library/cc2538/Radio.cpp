/*
 * Copyright 2013 OpenMote Technologies, S.L.
 */

/**
 *
 * @file       Radio.cpp
 * @author     Pere Tuset-Peiro (peretuset@openmote.com)
 * @version    v0.1
 * @date       May, 2014
 * @brief
 * @ingroup
 *
 */

/**********************************include************************************/

#include "Radio.h"

/*********************************variables***********************************/

Radio Radio::instance;

/**********************************public*************************************/

Radio & Radio::getInstance(void)
{
    return instance;
}

/*********************************protected***********************************/

void Radio::interruptHandler(void)
{
}

void Radio::errorHandler(void)
{
}

/**********************************private************************************/

Radio::Radio()
{    
}
