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

#include "Board.h"
#include "Button.h"
#include "Led.h"
#include "I2c.h"
#include "Uart.h"

#include "Adxl346.h"
#include "Max44009.h"
#include "Sht21.h"

/*================================ define ===================================*/

#define LED_RED_PORT            ( GPIO_C_BASE )
#define LED_RED_PIN             ( GPIO_PIN_4 )

#define LED_ORANGE_PORT         ( GPIO_C_BASE )
#define LED_ORANGE_PIN          ( GPIO_PIN_5 )

#define LED_YELLOW_PORT         ( GPIO_C_BASE )
#define LED_YELLOW_PIN          ( GPIO_PIN_6 )

#define LED_GREEN_PORT          ( GPIO_C_BASE )
#define LED_GREEN_PIN           ( GPIO_PIN_7 )

#define GPIO_DEBUG_AD0_PORT     ( GPIO_D_BASE )
#define GPIO_DEBUG_AD0_PIN      ( GPIO_PIN_3 )

#define GPIO_DEBUG_AD1_PORT     ( GPIO_D_BASE )
#define GPIO_DEBUG_AD1_PIN      ( GPIO_PIN_2 )

#define GPIO_DEBUG_AD2_PORT     ( GPIO_D_BASE )
#define GPIO_DEBUG_AD2_PIN      ( GPIO_PIN_1 )

#define BUTTON_USER_PORT        ( GPIO_C_BASE )
#define BUTTON_USER_PIN         ( GPIO_PIN_3 )
#define BUTTON_USER_EDGE        ( GPIO_FALLING_EDGE )

#define TPS62730_PORT           ( GPIO_B_BASE )
#define TPS62730_STAT_PIN       ( GPIO_PIN_0 )
#define TPS62730_ONBYP_PIN      ( GPIO_PIN_1 )

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

#define I2C_PERIPHERAL          ( SYS_CTRL_PERIPH_I2C )
#define I2C_PORT                ( GPIO_B_BASE )
#define I2C_SCL                 ( GPIO_PIN_3 )
#define I2C_SDA                 ( GPIO_PIN_4 )

/*================================ typedef ==================================*/

/*=============================== variables =================================*/

Board board;

Led tps62730(TPS62730_PORT, TPS62730_ONBYP_PIN);

Led led_green(LED_GREEN_PORT, LED_GREEN_PIN);
Led led_orange(LED_ORANGE_PORT, LED_ORANGE_PIN);
Led led_red(LED_RED_PORT, LED_RED_PIN);
Led led_yellow(LED_YELLOW_PORT, LED_YELLOW_PIN);

Button button_user(BUTTON_USER_PORT, BUTTON_USER_PIN, BUTTON_USER_EDGE);

// Gpio uart_rx(UART_RX_PORT, UART_RX_PIN);
// Gpio uart_tx(UART_TX_PORT, UART_TX_PIN);
// Uart uart(UART_PERIPHERAL, UART_PORT, UART_CLOCK, UART_INTERRUPT);

Gpio i2c_scl(I2C_PORT, I2C_SCL);
Gpio i2c_sda(I2C_PORT, I2C_SDA);
I2c i2c(I2C_PERIPHERAL, &i2c_scl, &i2c_sda);

Adxl346 adxl346(&i2c);
Max44009 max44009(&i2c);
Sht21 sht21(&i2c);

/*=============================== prototypes ================================*/

/*================================= public ==================================*/

/*================================ private ==================================*/

#endif /* BOARD_OPENMOTE_CC2538_H_ */
