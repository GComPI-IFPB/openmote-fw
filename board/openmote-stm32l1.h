/*
 * Copyright 2013 OpenMote Technologies, S.L.
 */

/**
 *
 * @file       openmote-stm32l1.h
 * @author     Pere Tuset-Peiro (peretuset@openmote.com)
 * @version    v0.1
 * @date       May, 2014
 * @brief
 * @ingroup
 *
 */
 
#ifndef BOARD_OPENMOTE_STM32L1_H_
#define BOARD_OPENMOTE_STM32L1_H_

/*================================ include ==================================*/

#include "stm32l1xx.h"

/*================================ define ===================================*/

#define LED_GREEN_PERIPH            ( RCC_AHBPeriph_GPIOC )
#define LED_GREEN_PORT              ( GPIOC )
#define LED_GREEN_PIN               ( GPIO_Pin_0 )
#define LED_GREEN_MODE              ( GPIO_Mode_OUT )
#define LED_GREEN_TYPE              ( GPIO_OType_PP )
#define LED_GREEN_SPEED             ( GPIO_Speed_40MHz )
#define LED_GREEN_PUSH              ( GPIO_PuPd_NOPULL )

#define LED_ORANGE_PERIPH           ( RCC_AHBPeriph_GPIOC )
#define LED_ORANGE_PORT             ( GPIOC )
#define LED_ORANGE_PIN              ( GPIO_Pin_1 )
#define LED_ORANGE_MODE             ( GPIO_Mode_OUT )
#define LED_ORANGE_TYPE             ( GPIO_OType_PP )
#define LED_ORANGE_SPEED            ( GPIO_Speed_40MHz )
#define LED_ORANGE_PUSH             ( GPIO_PuPd_NOPULL )

#define LED_RED_PERIPH              ( RCC_AHBPeriph_GPIOD )
#define LED_RED_PORT                ( GPIOD )
#define LED_RED_PIN                 ( GPIO_Pin_2 )
#define LED_RED_MODE                ( GPIO_Mode_OUT )
#define LED_RED_TYPE                ( GPIO_OType_PP )
#define LED_RED_SPEED               ( GPIO_Speed_40MHz )
#define LED_RED_PUSH                ( GPIO_PuPd_NOPULL )

#define LED_BLUE_PERIPH             ( RCC_AHBPeriph_GPIOC )
#define LED_BLUE_PORT               ( GPIOC )
#define LED_BLUE_PIN                ( GPIO_Pin_4 )
#define LED_BLUE_MODE               ( GPIO_Mode_OUT )
#define LED_BLUE_TYPE               ( GPIO_OType_PP )
#define LED_BLUE_SPEED              ( GPIO_Speed_40MHz )
#define LED_BLUE_PUSH               ( GPIO_PuPd_NOPULL )

#define LED_YELLOW_PERIPH           ( RCC_AHBPeriph_GPIOC )
#define LED_YELLOW_PORT             ( GPIOC )
#define LED_YELLOW_PIN              ( GPIO_Pin_5 )
#define LED_YELLOW_MODE             ( GPIO_Mode_OUT )
#define LED_YELLOW_TYPE             ( GPIO_OType_PP )
#define LED_YELLOW_SPEED            ( GPIO_Speed_40MHz )
#define LED_YELLOW_PUSH             ( GPIO_PuPd_NOPULL )

#define BUTTON_USER_PERIPH          ( RCC_AHBPeriph_GPIOC )
#define BUTTON_USER_PORT            ( GPIOC )
#define BUTTON_USER_PIN             ( GPIO_Pin_6 )
#define BUTTON_USER_MODE            ( GPIO_Mode_IN )
#define BUTTON_USER_TYPE            ( GPIO_OType_PP )
#define BUTTON_USER_SPEED           ( GPIO_Speed_40MHz )
#define BUTTON_USER_PUSH            ( GPIO_PuPd_NOPULL )
#define BUTTON_USER_EXTI_LINE       ( EXTI_Line6 )
#define BUTTON_USER_EXTI_MODE       ( EXTI_Mode_Interrupt )
#define BUTTON_USER_EXTI_TRIGGER    ( EXTI_Trigger_Falling )
#define BUTTON_USER_EXTI_CMD        ( ENABLE )
#define BUTTON_USER_NVIC_CHANNEL    ( EXTI6_IRQn )
#define BUTTON_USER_NVIC_PRIO       ( 0xF )
#define BUTTON_USER_NVIC_SUBPRIO    ( 0xF )
#define BUTTON_USER_NVIC_CMD        ( ENABLE )

/*================================ typedef ==================================*/

/*=============================== variables =================================*/

/*=============================== prototypes ================================*/

/*================================= public ==================================*/

/*================================ private ==================================*/

#endif /* BOARD_OPENMOTE_CC2538_H_ */
