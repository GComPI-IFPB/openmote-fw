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
#include "i2c.h"
#include "interrupt.h"
#include "ioc.h"
#include "sys_ctrl.h"
#include "uart.h"

#include "hw_gpio.h"
#include "hw_ints.h"
#include "hw_ioc.h"
#include "hw_memmap.h"
#include "hw_types.h"

#include "Board.h"
#include "GpioIn.h"
#include "GpioInPow.h"
#include "GpioOut.h"
#include "I2c.h"
#include "Uart.h"

#include "Adxl346.h"
#include "Max44009.h"
#include "Sht21.h"

/*================================ define ===================================*/

#define BOARD_HAS_32MHz_XTAL    ( )
#define BOARD_HAS_32kHz_XTAL    ( )
#define BOARD_CLOCK             ( SYS_CTRL_SYSDIV_16MHZ )

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

#define GPIO_DEBUG_AD3_PORT     ( GPIO_D_BASE )
#define GPIO_DEBUG_AD3_PIN      ( GPIO_PIN_0 )

#define BUTTON_USER_PORT        ( GPIO_C_BASE )
#define BUTTON_USER_PIN         ( GPIO_PIN_3 )
#define BUTTON_USER_EDGE        ( GPIO_FALLING_EDGE )

#define TPS62730_PORT           ( GPIO_B_BASE )
#define TPS62730_STATUS_PIN     ( GPIO_PIN_0 )
#define TPS62730_STATUS_EDGE    ( GPIO_BOTH_EDGES )
#define TPS62730_BYPASS_PIN     ( GPIO_PIN_1 )

#define UART_PERIPHERAL         ( SYS_CTRL_PERIPH_UART0 )
#define UART_BASE               ( UART0_BASE )
#define UART_CLOCK              ( UART_CLOCK_PIOSC )
#define UART_INTERRUPT          ( INT_UART0 )
#define UART_RX_PORT            ( GPIO_A_BASE )
#define UART_RX_PIN             ( GPIO_PIN_0 )
#define UART_RX_IOC             ( IOC_UARTRXD_UART0 )
#define UART_TX_PORT            ( GPIO_A_BASE )
#define UART_TX_PIN             ( GPIO_PIN_1 )
#define UART_TX_IOC             ( IOC_MUX_OUT_SEL_UART0_TXD )
#define UART_BAUDRATE           ( 115200 )
#define UART_CONFIG             ( UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE | UART_CONFIG_PAR_NONE )
#define UART_INT_MODE           ( UART_TXINT_MODE_EOT ) 

#define I2C_PERIPHERAL          ( SYS_CTRL_PERIPH_I2C )
#define I2C_BASE                ( GPIO_B_BASE )
#define I2C_SCL                 ( GPIO_PIN_3 )
#define I2C_SDA                 ( GPIO_PIN_4 )
#define I2C_BAUDRATE            ( 100000 )

#define ADXL346_INT_PORT        ( GPIO_B_BASE)
#define ADXL346_INT_PIN         ( GPIO_PIN_5 )
#define ADXL346_INT_EDGE        ( GPIO_FALLING_EDGE )

#define MAX44009_INT_PORT       ( GPIO_B_BASE)
#define MAX44009_INT_PIN        ( GPIO_PIN_2 )
#define MAX44009_INT_EDGE       ( GPIO_FALLING_EDGE )

/*================================ typedef ==================================*/

/*=============================== variables =================================*/

Board board;

GpioOut tps62730_bypass(TPS62730_PORT, TPS62730_BYPASS_PIN);
GpioIn  tps62730_status(TPS62730_PORT, TPS62730_STATUS_PIN, TPS62730_STATUS_EDGE);

GpioOut debug_ad0(GPIO_DEBUG_AD0_PORT, GPIO_DEBUG_AD0_PIN);
GpioOut debug_ad1(GPIO_DEBUG_AD1_PORT, GPIO_DEBUG_AD1_PIN);
GpioOut debug_ad2(GPIO_DEBUG_AD2_PORT, GPIO_DEBUG_AD2_PIN);
GpioOut debug_ad3(GPIO_DEBUG_AD3_PORT, GPIO_DEBUG_AD3_PIN);

GpioOut led_green(LED_GREEN_PORT, LED_GREEN_PIN);
GpioOut led_orange(LED_ORANGE_PORT, LED_ORANGE_PIN);
GpioOut led_red(LED_RED_PORT, LED_RED_PIN);
GpioOut led_yellow(LED_YELLOW_PORT, LED_YELLOW_PIN);

GpioInPow button_user(BUTTON_USER_PORT, BUTTON_USER_PIN, BUTTON_USER_EDGE);

Gpio uart_rx(UART_RX_PORT, UART_RX_PIN);
Gpio uart_tx(UART_TX_PORT, UART_TX_PIN);
Uart uart(UART_PERIPHERAL, UART_BASE, UART_CLOCK, UART_INTERRUPT, &uart_rx, UART_RX_IOC, &uart_tx, UART_TX_IOC );

Gpio i2c_scl(I2C_BASE, I2C_SCL);
Gpio i2c_sda(I2C_BASE, I2C_SDA);
I2c i2c(I2C_PERIPHERAL, &i2c_scl, &i2c_sda);

GpioIn adxl346_int(ADXL346_INT_PORT, ADXL346_INT_PIN, ADXL346_INT_EDGE);
Adxl346 adxl346(&i2c, &adxl346_int);

GpioIn max44009_int(MAX44009_INT_PORT, MAX44009_INT_PIN, MAX44009_INT_EDGE);
Max44009 max44009(&i2c, &max44009_int);

Sht21 sht21(&i2c);

/*=============================== prototypes ================================*/

/*================================= public ==================================*/

/*================================ private ==================================*/

#endif /* BOARD_OPENMOTE_CC2538_H_ */
