/*
 * Copyright 2013 OpenMote Technologies, S.L.
 */

/**
 *
 * @file       test-timer.cpp
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

/*=============================== variables =================================*/

/*=============================== prototypes ================================*/

static void timer0_callback(void);
static void timer1_callback(void);
static void timer2_callback(void);
static void timer3_callback(void);

static GenericCallback callback0(timer0_callback);
static GenericCallback callback1(timer1_callback);
static GenericCallback callback2(timer2_callback);
static GenericCallback callback3(timer3_callback);

/*================================= public ==================================*/

int main (void)
{
    timer0.init(800000);
    timer0.setCallback(&callback0);
    timer0.enableInterrupt();

    timer1.init(1600000);
    timer1.setCallback(&callback1);
    timer1.enableInterrupt();

    timer2.init(3200000);
    timer2.setCallback(&callback2);
    timer2.enableInterrupt();

    timer3.init(6400000);
    timer3.setCallback(&callback3);
    timer3.enableInterrupt();

    timer0.start();
    timer1.start();
    timer2.start();
    timer3.start();

    board.enableInterrupts();

    while(true)
    {
        board.sleep();
    }
}

/*=============================== protected =================================*/

/*================================ private ==================================*/

static void timer0_callback(void)
{
    led_green.toggle();
    debug_ad0.toggle();
}

static void timer1_callback(void)
{
    led_yellow.toggle();
    debug_ad1.toggle();
}

static void timer2_callback(void)
{
    led_orange.toggle();
    debug_ad2.toggle();
}

static void timer3_callback(void)
{
    led_red.toggle();
    debug_ad3.toggle();
}
