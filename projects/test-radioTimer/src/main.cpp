/*
 * Copyright 2013 OpenMote Technologies, S.L.
 */

/**
 *
 * @file       main.cpp
 * @author     Pere Tuset-Peiro (peretuset@openmote.com)
 * @version    v0.1
 * @date       May, 2014
 * @brief
 * @ingroup
 *
 */

/*================================ include ==================================*/

#include "openmote-cc2538.h"

#include "Callback.h"

/*================================ define ===================================*/

/*================================ typedef ==================================*/

/*=============================== prototypes ================================*/

static void sleep_timer_callback(void);

static PlainCallback sleepTimerCallback(sleep_timer_callback);

/*=============================== variables =================================*/

/*================================= public ==================================*/

int main (void)
{
    // Enable erasing the Flash with the user button
    board.enableFlashErase();

    // Initialize Rtc
    sleepTimer.init();
    sleepTimer.setCallback(&sleepTimerCallback);
    sleepTimer.enableInterrupts();

    // Start Rtc
    sleepTimer.start(32768);

    // Enable interrupts
    board.enableInterrupts();

    // Forever
    while (true)
    {
        // Sleep
        board.setSleepMode(SleepMode::SleepMode_2);
        board.sleep();
    }
}

/*=============================== protected =================================*/

/*================================ private ==================================*/

static void sleep_timer_callback(void)
{
    static bool led_status = false;

    if (led_status)
    {
        led_status = false;
        led_green.off();
        sleepTimer.start(32768);
    }
    else
    {
        led_status = true;
        led_green.on();
        sleepTimer.start(327);
    }
}
