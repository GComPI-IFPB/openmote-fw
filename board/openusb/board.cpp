/**
 * @file       board.cpp
 * @author     Pere Tuset-Peiro (peretuset@openmote.com)
 * @version    v0.1
 * @date       May, 2015
 * @brief
 *
 * @copyright  Copyright 2015, OpenMote Technologies, S.L.
 *             This file is licensed under the GNU General Public License v2.
 */

/*================================ include ==================================*/

#include "Aes.h"
#include "Board.h"
#include "Gpio.h"
#include "I2c.h"
#include "Radio.h"
#include "RadioTimer.h"
#include "SleepTimer.h"
#include "Spi.h"
#include "TemperatureSensor.h"
#include "Timer.h"
#include "Uart.h"
#include "RandomNumberGenerator.h"
#include "Watchdog.h"

#include "Adxl34x.h"
#include "Max44009.h"
#include "Si7006.h"

#include "Cc1200.h"

#include "cc2538_include.h"
#include "platform_types.h"

/*================================ define ===================================*/

#define BOARD_HAS_32MHz_XTAL    ( TRUE )
#define BOARD_USE_32MHz_XTAL    ( TRUE )
#define BOARD_HAS_32kHz_XTAL    ( TRUE )
#define BOARD_USE_32kHz_XTAL    ( TRUE )
#define SYSTEM_CLOCK_DIVIDER    ( SYS_CTRL_SYSDIV_32MHZ )
#define PERIPH_CLOCK_DIVIDER    ( SYS_CTRL_SYSDIV_16MHZ )
#define WATCHDOG_INTERVAL       ( WATCHDOG_INTERVAL_32768 )

#define LED_RED_PORT            ( GPIO_C_BASE )
#define LED_RED_PIN             ( GPIO_PIN_4 )
#define LED_RED_IOC             ( IOC_MUX_OUT_SEL_GPT0_ICP1 )

#define LED_ORANGE_PORT         ( GPIO_C_BASE )
#define LED_ORANGE_PIN          ( GPIO_PIN_5 )
#define LED_ORANGE_IOC          ( IOC_MUX_OUT_SEL_GPT0_ICP1 )

#define LED_YELLOW_PORT         ( GPIO_C_BASE )
#define LED_YELLOW_PIN          ( GPIO_PIN_6 )
#define LED_YELLOW_IOC          ( IOC_MUX_OUT_SEL_GPT0_ICP1 )

#define LED_GREEN_PORT          ( GPIO_C_BASE )
#define LED_GREEN_PIN           ( GPIO_PIN_7 )
#define LED_GREEN_IOC           ( IOC_MUX_OUT_SEL_GPT0_ICP1 )

#define GPIO_DEBUG_AD0_PORT     ( GPIO_D_BASE )
#define GPIO_DEBUG_AD0_PIN      ( GPIO_PIN_3 )

#define GPIO_DEBUG_AD1_PORT     ( GPIO_D_BASE )
#define GPIO_DEBUG_AD1_PIN      ( GPIO_PIN_2 )

#define GPIO_DEBUG_AD2_PORT     ( GPIO_D_BASE )
#define GPIO_DEBUG_AD2_PIN      ( GPIO_PIN_1 )

#define BUTTON_USER_PORT        ( GPIO_C_BASE )
#define BUTTON_USER_PIN         ( GPIO_PIN_3 )
#define BUTTON_USER_EDGE        ( GPIO_FALLING_EDGE )

#define BOOTLOAD_PORT           ( GPIO_A_BASE )
#define BOOTLOAD_PIN            ( GPIO_PIN_6 )

#define TIMER0A_PERIPHERAL      ( SYS_CTRL_PERIPH_GPT0 )
#define TIMER0A_BASE            ( GPTIMER0_BASE )
#define TIMER0A_SOURCE          ( GPTIMER_A )
#define TIMER0A_CONFIG          ( GPTIMER_CFG_SPLIT_PAIR |  GPTIMER_CFG_A_PWM | GPTIMER_CFG_B_PERIODIC )
#define TIMER0A_INTERRUPT       ( INT_TIMER0A )
#define TIMER0A_INTERRUPT_MODE  ( GPTIMER_TIMA_TIMEOUT )

#define TIMER0B_PERIPHERAL      ( SYS_CTRL_PERIPH_GPT0 )
#define TIMER0B_BASE            ( GPTIMER0_BASE )
#define TIMER0B_SOURCE          ( GPTIMER_B )
#define TIMER0B_CONFIG          ( GPTIMER_CFG_SPLIT_PAIR |  GPTIMER_CFG_A_PWM | GPTIMER_CFG_B_PERIODIC )
#define TIMER0B_INTERRUPT       ( INT_TIMER0B )
#define TIMER0B_INTERRUPT_MODE  ( GPTIMER_TIMB_TIMEOUT )

#define TIMER1A_PERIPHERAL      ( SYS_CTRL_PERIPH_GPT1 )
#define TIMER1A_BASE            ( GPTIMER1_BASE )
#define TIMER1A_SOURCE          ( GPTIMER_A )
#define TIMER1A_CONFIG          ( GPTIMER_CFG_SPLIT_PAIR |  GPTIMER_CFG_A_PERIODIC | GPTIMER_CFG_B_PERIODIC )
#define TIMER1A_INTERRUPT       ( INT_TIMER1A )
#define TIMER1A_INTERRUPT_MODE  ( GPTIMER_TIMA_TIMEOUT )

#define TIMER1B_PERIPHERAL      ( SYS_CTRL_PERIPH_GPT1 )
#define TIMER1B_BASE            ( GPTIMER1_BASE )
#define TIMER1B_SOURCE          ( GPTIMER_B )
#define TIMER1B_CONFIG          ( GPTIMER_CFG_SPLIT_PAIR |  GPTIMER_CFG_A_PERIODIC | GPTIMER_CFG_B_PERIODIC )
#define TIMER1B_INTERRUPT       ( INT_TIMER1B )
#define TIMER1B_INTERRUPT_MODE  ( GPTIMER_TIMB_TIMEOUT )

#define TIMER2A_PERIPHERAL      ( SYS_CTRL_PERIPH_GPT2 )
#define TIMER2A_BASE            ( GPTIMER2_BASE )
#define TIMER2A_SOURCE          ( GPTIMER_A )
#define TIMER2A_CONFIG          ( GPTIMER_CFG_SPLIT_PAIR |  GPTIMER_CFG_A_PERIODIC | GPTIMER_CFG_B_PERIODIC )
#define TIMER2A_INTERRUPT       ( INT_TIMER2A )
#define TIMER2A_INTERRUPT_MODE  ( GPTIMER_TIMA_TIMEOUT )

#define TIMER2B_PERIPHERAL      ( SYS_CTRL_PERIPH_GPT2 )
#define TIMER2B_BASE            ( GPTIMER2_BASE )
#define TIMER2B_SOURCE          ( GPTIMER_B )
#define TIMER2B_CONFIG          ( GPTIMER_CFG_SPLIT_PAIR |  GPTIMER_CFG_A_PERIODIC | GPTIMER_CFG_B_PERIODIC )
#define TIMER2B_INTERRUPT       ( INT_TIMER2B )
#define TIMER2B_INTERRUPT_MODE  ( GPTIMER_TIMB_TIMEOUT )

#define TIMER3A_PERIPHERAL      ( SYS_CTRL_PERIPH_GPT3 )
#define TIMER3A_BASE            ( GPTIMER3_BASE )
#define TIMER3A_SOURCE          ( GPTIMER_A )
#define TIMER3A_CONFIG          ( GPTIMER_CFG_SPLIT_PAIR |  GPTIMER_CFG_A_PERIODIC | GPTIMER_CFG_B_PERIODIC )
#define TIMER3A_INTERRUPT       ( INT_TIMER3A )
#define TIMER3A_INTERRUPT_MODE  ( GPTIMER_TIMA_TIMEOUT )

#define TIMER3B_PERIPHERAL      ( SYS_CTRL_PERIPH_GPT3 )
#define TIMER3B_BASE            ( GPTIMER3_BASE )
#define TIMER3B_SOURCE          ( GPTIMER_B )
#define TIMER3B_CONFIG          ( GPTIMER_CFG_SPLIT_PAIR |  GPTIMER_CFG_A_PERIODIC | GPTIMER_CFG_B_PERIODIC )
#define TIMER3B_INTERRUPT       ( INT_TIMER3B )
#define TIMER3B_INTERRUPT_MODE  ( GPTIMER_TIMB_TIMEOUT )

#define SLEEP_TIMER_INTERRUPT   ( INT_SMTIM )

#define RADIO_TIMER_INTERRUPT   ( INT_MACTIMR )

#define ANTENNA_EXTERNAL_PORT   ( GPIO_D_BASE )
#define ANTENNA_EXTERNAL_PIN    ( GPIO_PIN_4 )
#define ANTENNA_INTERNAL_PORT   ( GPIO_D_BASE )
#define ANTENNA_INTERNAL_PIN    ( GPIO_PIN_5 )

#define UART_PERIPHERAL         ( SYS_CTRL_PERIPH_UART0 )
#define UART_BASE               ( UART0_BASE )
#define UART_CLOCK              ( UART_CLOCK_PIOSC )
#define UART_INT                ( INT_UART0 )
#define UART_BAUDRATE           ( 115200 )
#define UART_MODE               ( UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE | UART_CONFIG_PAR_NONE )

#define UART_RX_PORT            ( GPIO_A_BASE )
#define UART_RX_PIN             ( GPIO_PIN_0 )
#define UART_RX_IOC             ( IOC_UARTRXD_UART0 )
#define UART_TX_PORT            ( GPIO_A_BASE )
#define UART_TX_PIN             ( GPIO_PIN_1 )
#define UART_TX_IOC             ( IOC_MUX_OUT_SEL_UART0_TXD )

#define SPI_PERIPHERAL          ( SYS_CTRL_PERIPH_SSI0 )
#define SPI_BASE                ( SSI0_BASE )
#define SPI_CLOCK               ( SSI_CLOCK_PIOSC )
#define SPI_INT                 ( INT_SSI0 )
#define SPI_MODE                ( SSI_MODE_MASTER )
#define SPI_PROTOCOL            ( SSI_FRF_MOTO_MODE_0 )
#define SPI_DATAWIDTH           ( 8 )
#define SPI_BAUDRATE            ( 8000000 )

#define SPI_MISO_BASE           ( GPIO_A_BASE )
#define SPI_MISO_PIN            ( GPIO_PIN_4 )
#define SPI_MISO_IOC            ( IOC_SSIRXD_SSI0 )
#define SPI_MOSI_BASE           ( GPIO_A_BASE )
#define SPI_MOSI_PIN            ( GPIO_PIN_5 )
#define SPI_MOSI_IOC            ( IOC_MUX_OUT_SEL_SSI0_TXD )
#define SPI_CLK_BASE            ( GPIO_A_BASE )
#define SPI_CLK_PIN             ( GPIO_PIN_2 )
#define SPI_CLK_IOC             ( IOC_MUX_OUT_SEL_SSI0_CLKOUT )

#define I2C_PERIPHERAL          ( SYS_CTRL_PERIPH_I2C )
#define I2C_BAUDRATE            ( 400000 )
#define I2C_SCL_BASE            ( GPIO_B_BASE )
#define I2C_SCL_PIN             ( GPIO_PIN_3 )
#define I2C_SDA_BASE            ( GPIO_B_BASE )
#define I2C_SDA_PIN             ( GPIO_PIN_4 )

#define ADXL34X_INT_PORT        ( GPIO_B_BASE)
#define ADXL34X_INT_PIN         ( GPIO_PIN_2 )
#define ADXL34X_INT_EDGE        ( GPIO_RISING_EDGE )

#define MAX44009_INT_PORT       ( GPIO_B_BASE )
#define MAX44009_INT_PIN        ( GPIO_PIN_5 )
#define MAX44009_INT_EDGE       ( GPIO_FALLING_EDGE )

#define CC1200_CS_BASE          ( GPIO_A_BASE )
#define CC1200_CS_PIN           ( GPIO_PIN_3 )
#define CC1200_CS_IOC           ( IOC_MUX_OUT_SEL_SSI0_FSSOUT )

#define CC1200_GPIO0_BASE       ( GPIO_D_BASE )
#define CC1200_GPIO0_PIN        ( GPIO_PIN_3 )
#define CC1200_GPIO0_EDGE       ( GPIO_RISING_EDGE )

#define CC1200_GPIO2_BASE       ( GPIO_D_BASE )
#define CC1200_GPIO2_PIN        ( GPIO_PIN_2 )
#define CC1200_GPIO2_EDGE       ( GPIO_RISING_EDGE )

#define CC1200_GPIO3_BASE       ( GPIO_D_BASE )
#define CC1200_GPIO3_PIN        ( GPIO_PIN_0 )
#define CC1200_GPIO3_EDGE       ( GPIO_RISING_EDGE )

/*================================ typedef ==================================*/

/*=============================== variables =================================*/

// Board management
Board board;
Watchdog watchdog(WATCHDOG_INTERVAL);

// Timers
TimerConfig timer0a_cfg = {TIMER0A_PERIPHERAL, TIMER0A_BASE, TIMER0A_SOURCE, TIMER0A_CONFIG, TIMER0A_INTERRUPT, TIMER0A_INTERRUPT_MODE};
Timer timer0a(timer0a_cfg);
TimerConfig timer0b_cfg = {TIMER0B_PERIPHERAL, TIMER0B_BASE, TIMER0B_SOURCE, TIMER0B_CONFIG, TIMER0B_INTERRUPT, TIMER0B_INTERRUPT_MODE};
Timer timer0b(timer0b_cfg);

TimerConfig timer1a_cfg = {TIMER1A_PERIPHERAL, TIMER1A_BASE, TIMER1A_SOURCE, TIMER1A_CONFIG, TIMER1A_INTERRUPT, TIMER1A_INTERRUPT_MODE};
Timer timer1a(timer1a_cfg);
TimerConfig timer1b_cfg = {TIMER1B_PERIPHERAL, TIMER1B_BASE, TIMER1B_SOURCE, TIMER1B_CONFIG, TIMER1B_INTERRUPT, TIMER1B_INTERRUPT_MODE};
Timer timer1b(timer1b_cfg);

TimerConfig timer2a_cfg = {TIMER2A_PERIPHERAL, TIMER2A_BASE, TIMER2A_SOURCE, TIMER2A_CONFIG, TIMER2A_INTERRUPT, TIMER2A_INTERRUPT_MODE};
Timer timer2a(timer2a_cfg);
TimerConfig timer2b_cfg = {TIMER2B_PERIPHERAL, TIMER2B_BASE, TIMER2B_SOURCE, TIMER2B_CONFIG, TIMER2B_INTERRUPT, TIMER2B_INTERRUPT_MODE};
Timer timer2b(timer2b_cfg);

TimerConfig timer3a_cfg = {TIMER3A_PERIPHERAL, TIMER3A_BASE, TIMER3A_SOURCE, TIMER3A_CONFIG, TIMER3A_INTERRUPT, TIMER3A_INTERRUPT_MODE};
Timer timer3a(timer3a_cfg);
TimerConfig timer3b_cfg = {TIMER3B_PERIPHERAL, TIMER3B_BASE, TIMER3B_SOURCE, TIMER3B_CONFIG, TIMER3B_INTERRUPT, TIMER3B_INTERRUPT_MODE};
Timer timer3b(timer3b_cfg);

// SleepTimer
SleepTimer sleepTimer(SLEEP_TIMER_INTERRUPT);

// RadioTimer
RadioTimer radioTimer(RADIO_TIMER_INTERRUPT);

// Leds
// GpioConfig led_green_cfg = {LED_GREEN_PORT, LED_GREEN_PIN, 0, 0, 0};
// GpioOut led_green(led_green_cfg);

// GpioConfig led_orange_cfg = {LED_ORANGE_PORT, LED_ORANGE_PIN, 0, 0, 0};
// GpioOut led_orange(led_orange_cfg);

// GpioConfig led_red_cfg = {LED_RED_PORT, LED_RED_PIN, 0, 0, 0};
// GpioOut led_red(led_red_cfg);

//GpioConfig led_yellow_cfg = {LED_YELLOW_PORT, LED_YELLOW_PIN, 0, 0, 0};
// GpioOut led_yellow(led_yellow_cfg);

// PWM Leds
GpioConfig led_green_cfg = {LED_GREEN_PORT, LED_GREEN_PIN, LED_GREEN_IOC, 0, 0};
GpioPwm led_green(led_green_cfg, timer0a_cfg);

GpioConfig led_orange_cfg = {LED_ORANGE_PORT, LED_ORANGE_PIN, LED_ORANGE_IOC, 0, 0};
GpioPwm led_orange(led_orange_cfg, timer1a_cfg);

GpioConfig led_red_cfg = {LED_RED_PORT, LED_RED_PIN, LED_RED_IOC, 0, 0};
GpioPwm led_red(led_red_cfg, timer2a_cfg);

GpioConfig led_yellow_cfg = {LED_YELLOW_PORT, LED_YELLOW_PIN, LED_YELLOW_IOC, 0, 0};
GpioPwm led_yellow(led_yellow_cfg, timer3a_cfg);

// Button
GpioConfig button_user_cfg = {BUTTON_USER_PORT, BUTTON_USER_PIN, 0, BUTTON_USER_EDGE, 0};
GpioInPow button_user(button_user_cfg);

// I2C peripheral
GpioConfig i2c_scl_cfg = {I2C_SCL_BASE, I2C_SCL_PIN, 0, 0, 0};
GpioConfig i2c_sda_cfg = {I2C_SDA_BASE, I2C_SDA_PIN, 0, 0, 0};
I2cConfig i2c_cfg      = {I2C_PERIPHERAL, I2C_BAUDRATE};
Gpio i2c_scl(i2c_scl_cfg);
Gpio i2c_sda(i2c_sda_cfg);
I2c i2c(i2c_scl, i2c_sda, i2c_cfg);

// SPI peripheral
GpioConfig spi_miso_cfg = {SPI_MISO_BASE, SPI_MISO_PIN, SPI_MISO_IOC, 0, 0};
GpioConfig spi_mosi_cfg = {SPI_MOSI_BASE, SPI_MOSI_PIN, SPI_MOSI_IOC, 0, 0};
GpioConfig spi_clk_cfg  = {SPI_CLK_BASE, SPI_CLK_PIN, SPI_CLK_IOC, 0, 0};
SpiConfig spi_cfg       = {SPI_PERIPHERAL, SPI_BASE, SPI_CLOCK, SPI_INT, SPI_MODE, SPI_PROTOCOL, SPI_DATAWIDTH, SPI_BAUDRATE};
Gpio spi_miso(spi_miso_cfg);
Gpio spi_mosi(spi_mosi_cfg);
Gpio spi_clk(spi_clk_cfg);
Spi spi(spi_miso, spi_mosi, spi_clk, spi_cfg);

// UART peripheral
GpioConfig uart_rx_cfg = {UART_RX_PORT, UART_RX_PIN, UART_RX_IOC, 0, 0};
GpioConfig uart_tx_cfg = {UART_TX_PORT, UART_TX_PIN, UART_TX_IOC, 0, 0};
UartConfig uart_cfg = {UART_PERIPHERAL, UART_BASE, UART_CLOCK, UART_INT, UART_BAUDRATE, UART_MODE};
Gpio uart_rx(uart_rx_cfg);
Gpio uart_tx(uart_tx_cfg);
Uart uart(uart_rx, uart_tx, uart_cfg);

// Random Number Generator peripheral
RandomNumberGenerator rng;

// IEEE 802.15.4 radio
Radio radio;

// AES module
Aes aes;

// Acceleration sensor
GpioConfig adxl34x_int_cfg = {ADXL34X_INT_PORT, ADXL34X_INT_PIN, 0, ADXL34X_INT_EDGE, 0};
GpioInPow adxl34x_int(adxl34x_int_cfg);
Adxl34x adxl34x(i2c, adxl34x_int);

// Light sensor
GpioConfig max44009_int_cfg = {MAX44009_INT_PORT, MAX44009_INT_PIN, 0, MAX44009_INT_EDGE, 0};
GpioInPow max44009_int(max44009_int_cfg);
Max44009 max44009(i2c, max44009_int);

// Temperature + Relative humidity sensor
Si7006 si7006(i2c);

// CC2538 Temperature sensor
TemperatureSensor temp;

// CC1200 radio transceiver
GpioConfig cc1200_cs_cfg    = {CC1200_CS_BASE, CC1200_CS_PIN, CC1200_CS_IOC, 0, 0};
GpioConfig cc1200_gpio0_cfg = {CC1200_GPIO0_BASE, CC1200_GPIO0_PIN, 0, CC1200_GPIO0_EDGE, 0};
GpioConfig cc1200_gpio2_cfg = {CC1200_GPIO2_BASE, CC1200_GPIO2_PIN, 0, CC1200_GPIO2_EDGE, 0};
GpioConfig cc1200_gpio3_cfg = {CC1200_GPIO3_BASE, CC1200_GPIO3_PIN, 0, CC1200_GPIO3_EDGE, 0};

GpioOut cc1200_cs(cc1200_cs_cfg);
GpioIn cc1200_gpio0(cc1200_gpio0_cfg);
GpioIn cc1200_gpio2(cc1200_gpio2_cfg);
GpioIn cc1200_gpio3(cc1200_gpio3_cfg);
Cc1200 cc1200(spi, cc1200_cs, cc1200_gpio0, cc1200_gpio2, cc1200_gpio3);

/*=============================== prototypes ================================*/

/*================================= public ==================================*/

void Board::init(void) {
}

/*================================ private ==================================*/