/*
 * Copyright 2013 OpenMote Technologies, S.L.
 */

/**
 *
 * @file       Watchdog.cpp
 * @author     Pere Tuset-Peiro (peretuset@openmote.com)
 * @version    v0.1
 * @date       May, 2014
 * @brief
 * @ingroup
 *
 */

/*================================ include ==================================*/

#include "Watchdog.h"

#include "cc2538_include.h"

/*================================ define ===================================*/

/*================================ typedef ==================================*/

/*=============================== variables =================================*/

/*=============================== prototypes ================================*/

/*================================= public ==================================*/

Watchdog::Watchdog(uint32_t interval_):
    interval(interval_)
{
}

void Watchdog::init(void)
{
    WatchdogEnable(interval);
}

void Watchdog::walk(void)
{
    WatchdogClear();
}

/*=============================== protected =================================*/

/*================================ private ==================================*/
