/*
 * Copyright 2013 OpenMote Technologies, S.L.
 */

/**
 *
 * @file       openmote-cc2538.h
 * @author     Pere Tuset-Peiro (peretuset@openmote.com)
 * @version    v0.1
 * @date       May, 2014
 * @brief
 * @ingroup
 *
 */
 
#ifndef BOARD_OPENMOTE_CC2538_H_
#define BOARD_OPENMOTE_CC2538_H_

/*================================ include ==================================*/

#include "gpio.h"

#include "hw_gpio.h"
#include "hw_types.h"
#include "hw_memmap.h"

/*================================ define ===================================*/

#define LED_RED_PORT            ( GPIO_C_BASE )
#define LED_RED_PIN             ( GPIO_PIN_4 )

#define LED_ORANGE_PORT         ( GPIO_C_BASE )
#define LED_ORANGE_PIN          ( GPIO_PIN_5 )

#define LED_YELLOW_PORT         ( GPIO_C_BASE )
#define LED_YELLOW_PIN          ( GPIO_PIN_6 )

#define LED_GREEN_PORT          ( GPIO_C_BASE )
#define LED_GREEN_PIN           ( GPIO_PIN_7 )

#define BUTTON_USER_PORT        ( GPIO_C_BASE )
#define BUTTON_USER_PIN         ( GPIO_PIN_3 )
#define BUTTON_USER_EDGE        ( GPIO_FALLING_EDGE )

#define UART_PERIPHERAL         ( SYS_CTRL_PERIPH_UART0 )
#define UART_PORT               ( UART0_BASE )
#define UART_CLOCK              ( UART_CLOCK_PIOSC )
#define UART_INTERRUPT          ( INT_UART0 )
#define UART_BAUDRATE           ( 115200 )
#define UART_CONFIG             ( UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE | UART_CONFIG_PAR_NONE )
#define UART_RX_PORT            ( GPIO_A_BASE )
#define UART_RX_PIN             ( GPIO_PIN_0 )
#define UART_RX_IOC             ( IOC_UARTRXD_UART0 )
#define UART_RX_INT             ( UART_INT_RX )
#define UART_TX_PORT            ( GPIO_A_BASE )
#define UART_TX_PIN             ( GPIO_PIN_1 )
#define UART_TX_IOC             ( IOC_MUX_OUT_SEL_UART0_TXD )
#define UART_TX_INT             ( UART_INT_TX )
#define UART_TX_INT_MODE        ( UART_TXINT_MODE_EOT ) 

/*================================ typedef ==================================*/

/*=============================== variables =================================*/

/*=============================== prototypes ================================*/

/*================================= public ==================================*/

/*================================ private ==================================*/

#endif /* BOARD_OPENMOTE_CC2538_H_ */
