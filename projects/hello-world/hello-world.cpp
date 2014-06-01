/*
 * Copyright 2013 OpenMote Technologies, S.L.
 */

/**
 *
 * @file       hello-world.c
 * @author     Pere Tuset-Peiro (peretuset@openmote.com)
 * @version    v0.1
 * @date       May, 2014
 * @brief
 * @ingroup
 *
 */


/*================================ include ==================================*/

#include "Led.h"
#include "Button.h"

#include "openmote-cc2538.h"

/*================================ define ===================================*/

#define WAIT_TICKS      ( 320000 )

/*================================ typedef ==================================*/

void button_user_callback(void);

/*=============================== variables =================================*/

Led led_green(LED_GREEN_PORT, LED_GREEN_PIN);
Led led_orange(LED_ORANGE_PORT, LED_ORANGE_PIN);
Led led_red(LED_RED_PORT, LED_RED_PIN);
Led led_yellow(LED_YELLOW_PORT, LED_YELLOW_PIN);

Button button_user(BUTTON_USER_PORT, BUTTON_USER_PIN, BUTTON_USER_EDGE);

/*=============================== prototypes ================================*/

/*================================= public ==================================*/

void button_user_callback(void)
{
    led_yellow.toggle();
    led_orange.toggle();
}

int main(void)
{
    volatile uint32_t i = 0;

    led_green.on();
    led_red.off();

    button_user.setCallback(button_user_callback);
    button_user.enableInterrupt();
    
    while(true) {
        led_green.toggle();
        led_red.toggle();
        for(i = 0; i < WAIT_TICKS; i++) {
        }
    }
    return 0;
}

/*================================ private ==================================*/


