/*
 * Copyright 2013 OpenMote Technologies, S.L.
 */

/**
 *
 * @file       SerialManager.cpp
 * @author     Pere Tuset-Peiro (peretuset@openmote.com)
 * @version    v0.1
 * @date       May, 2014
 * @brief
 * @ingroup
 *
 */

/*================================ include ==================================*/

#include "SerialManager.h"

/*================================ define ===================================*/

/*================================ typedef ==================================*/

/*=============================== variables =================================*/

/*=============================== prototypes ================================*/

/*================================= public ==================================*/

SerialManager::SerialManager(Serial& serial_):
    serial(serial_)
{
}

void SerialManager::init(void)
{
}

void SerialManager::registerCallback(void)
{
}

/*=============================== protected =================================*/

/*================================ private ==================================*/
