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
#include "ssi.h"
#include "gptimer.h"

#include "hw_gpio.h"
#include "hw_ints.h"
#include "hw_ioc.h"
#include "hw_memmap.h"
#include "hw_types.h"

#include "Board.h"
#include "GpioIn.h"
#include "GpioInPow.h"
#include "GpioOut.h"

#include "I2cDriver.h"
#include "SpiDriver.h"
#include "UartDriver.h"

#include "Adxl346.h"
#include "Max44009.h"
#include "Sht21.h"

#include "Tps62730.h"
#include "Enc28j60.h"

/*================================ define ===================================*/

#define BOARD_HAS_32MHz_XTAL    ( TRUE )
#define BOARD_HAS_32kHz_XTAL    ( TRUE )
#define BOARD_USE_32MHz_XTAL    ( FALSE )
#define BOARD_USE_32kHz_XTAL    ( TRUE )
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

#define SPI_PERIPHERAL          ( SYS_CTRL_PERIPH_SSI0 )
#define SPI_BASE                ( SSI0_BASE )
#define SPI_CLOCK               ( SSI_CLOCK_PIOSC )
#define SPI_INTERRUPT           ( INT_SSI0 )
#define SPI_MISO_BASE           ( GPIO_A_BASE )
#define SPI_MISO_PIN            ( GPIO_PIN_5 )
#define SPI_MISO_IOC            ( IOC_SSIRXD_SSI0 )
#define SPI_MOSI_BASE           ( GPIO_A_BASE )
#define SPI_MOSI_PIN            ( GPIO_PIN_4 )
#define SPI_MOSI_IOC            ( IOC_MUX_OUT_SEL_SSI0_TXD )
#define SPI_nCS_BASE            ( GPIO_A_BASE )
#define SPI_nCS_PIN             ( GPIO_PIN_3 )
#define SPI_nCS_IOC             ( IOC_MUX_OUT_SEL_SSI0_FSSOUT )
#define SPI_CLK_BASE            ( GPIO_A_BASE )
#define SPI_CLK_PIN             ( GPIO_PIN_2 )
#define SPI_CLK_IOC             ( IOC_MUX_OUT_SEL_SSI0_CLKOUT )
#define SPI_MODE                ( SSI_MODE_MASTER )
#define SPI_PROTOCOL            ( SSI_FRF_MOTO_MODE_3 )
#define SPI_DATAWIDTH           ( 8 )
#define SPI_BAUDRATE            ( 1000000 )

#define TIMER_PERIPHERAL        ( GPTIMER0_BASE )
#define TIMER_MODE              ( GPTIMER_BOTH )
#define TIMER_SPLIT             ( GPTIMER_CFG_SPLIT_PAIR )
#define TIMER_SHOT              ( GPTIMER_CFG_A_ONE_SHOT )
#define TIMER_COUNT

#define I2C_PERIPHERAL          ( SYS_CTRL_PERIPH_I2C )
#define I2C_BASE                ( GPIO_B_BASE )
#define I2C_SCL                 ( GPIO_PIN_3 )
#define I2C_SDA                 ( GPIO_PIN_4 )
#define I2C_BAUDRATE            ( 100000 )

#define ADXL346_INT_PORT        ( GPIO_B_BASE)
#define ADXL346_INT_PIN         ( GPIO_PIN_5 )
#define ADXL346_INT_EDGE        ( GPIO_FALLING_EDGE )

#define MAX44009_INT_PORT       ( GPIO_B_BASE )
#define MAX44009_INT_PIN        ( GPIO_PIN_2 )
#define MAX44009_INT_EDGE       ( GPIO_FALLING_EDGE )

#define ENC28J60_INT_PORT       ( GPIO_B_BASE )
#define ENC28J60_INT_PIN        ( GPIO_PIN_2 )
#define ENC28J60_INT_EDGE       ( GPIO_FALLING_EDGE )

/*================================ typedef ==================================*/

/*=============================== variables =================================*/

Board board;

GpioOut bypass(TPS62730_PORT, TPS62730_BYPASS_PIN);
GpioIn  status(TPS62730_PORT, TPS62730_STATUS_PIN, TPS62730_STATUS_EDGE);
Tps62730 tps62730(&bypass, &status);

GpioOut debug_ad0(GPIO_DEBUG_AD0_PORT, GPIO_DEBUG_AD0_PIN);
GpioOut debug_ad1(GPIO_DEBUG_AD1_PORT, GPIO_DEBUG_AD1_PIN);
GpioOut debug_ad2(GPIO_DEBUG_AD2_PORT, GPIO_DEBUG_AD2_PIN);
GpioOut debug_ad3(GPIO_DEBUG_AD3_PORT, GPIO_DEBUG_AD3_PIN);

GpioOut led_green(LED_GREEN_PORT, LED_GREEN_PIN);
GpioOut led_orange(LED_ORANGE_PORT, LED_ORANGE_PIN);
GpioOut led_red(LED_RED_PORT, LED_RED_PIN);
GpioOut led_yellow(LED_YELLOW_PORT, LED_YELLOW_PIN);

GpioInPow button_user(BUTTON_USER_PORT, BUTTON_USER_PIN, BUTTON_USER_EDGE);

GpioI2c i2c_scl(I2C_BASE, I2C_SCL);
GpioI2c i2c_sda(I2C_BASE, I2C_SDA);
I2cDriver i2c(I2C_PERIPHERAL, &i2c_scl, &i2c_sda);

GpioSpi spi_miso(SPI_MISO_BASE, SPI_MISO_PIN, SPI_MISO_IOC);
GpioSpi spi_mosi(SPI_MOSI_BASE, SPI_MOSI_PIN, SPI_MOSI_IOC);
GpioSpi spi_clk(SPI_CLK_BASE, SPI_CLK_PIN, SPI_CLK_IOC);
GpioSpi spi_ncs(SPI_nCS_BASE, SPI_nCS_PIN, SPI_nCS_IOC);
SpiDriver spi(SPI_PERIPHERAL, SPI_BASE, SPI_CLOCK, &spi_miso, &spi_mosi, &spi_clk, &spi_ncs);

GpioUart uart_rx(UART_RX_PORT, UART_RX_PIN, UART_RX_IOC);
GpioUart uart_tx(UART_TX_PORT, UART_TX_PIN, UART_TX_IOC);
UartDriver uart(UART_PERIPHERAL, UART_BASE, UART_CLOCK, UART_INTERRUPT, &uart_rx, &uart_tx);

GpioInPow adxl346_int(ADXL346_INT_PORT, ADXL346_INT_PIN, ADXL346_INT_EDGE);
Adxl346 adxl346(&i2c, &adxl346_int);

GpioIn max44009_int(MAX44009_INT_PORT, MAX44009_INT_PIN, MAX44009_INT_EDGE);
Max44009 max44009(&i2c, &max44009_int);

Sht21 sht21(&i2c);

GpioIn enc28j60_int(ENC28J60_INT_PORT, ENC28J60_INT_PIN, ENC28J60_INT_EDGE);
Enc28j60 enc28j60(&spi, &enc28j60_int);

/*=============================== prototypes ================================*/

/*================================= public ==================================*/

/*================================ private ==================================*/

#endif /* BOARD_OPENMOTE_CC2538_H_ */
