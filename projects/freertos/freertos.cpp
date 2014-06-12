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
#include "Board.h"
#include "Button.h"
#include "Uart.h"
#include "I2c.h"
#include "Adxl346.h"
#include "Max44009.h"
#include "Sht21.h"

#include "FreeRTOS.h"
#include "task.h"

#include "openmote-cc2538.h"

/*================================ define ===================================*/

#define mainCHECK_TASK_PRIORITY				( tskIDLE_PRIORITY + 3 )

/*================================ typedef ==================================*/

/*=============================== variables =================================*/

Board board;

Led led_green(LED_GREEN_PORT, LED_GREEN_PIN);
Led led_orange(LED_ORANGE_PORT, LED_ORANGE_PIN);
Led led_red(LED_RED_PORT, LED_RED_PIN);
Led led_yellow(LED_YELLOW_PORT, LED_YELLOW_PIN);

Button button_user(BUTTON_USER_PORT, BUTTON_USER_PIN, BUTTON_USER_EDGE);

Gpio uart_rx(UART_RX_PORT, UART_RX_PIN);
Gpio uart_tx(UART_TX_PORT, UART_TX_PIN);
Uart uart(UART_PERIPHERAL, UART_PORT, UART_CLOCK, UART_INTERRUPT);

Gpio i2c_scl(I2C_PORT, I2C_SCL);
Gpio i2c_sda(I2C_PORT, I2C_SDA);
I2c i2c(I2C_PERIPHERAL, &i2c_scl, &i2c_sda);

Adxl346 adxl346(&i2c);
Max44009 max44009(&i2c);
Sht21 sht21(&i2c);

/*=============================== prototypes ================================*/

static void prvRedLedTask(void *pvParameters);

/*================================= public ==================================*/

static void prvRedLedTask( void *pvParameters ) {
	while(true) {
		led_red.toggle();
		vTaskDelay(500 / portTICK_RATE_MS);
	}
}

int main (void) {
    led_green.on();
    led_red.on();
    
    uart.setRxGpio(uart_rx, UART_RX_IOC);
    uart.setTxGpio(uart_tx, UART_TX_IOC);
    uart.init(UART_BAUDRATE, UART_CONFIG, UART_TX_INT_MODE);
    
    i2c.init(100000);

	xTaskCreate(prvRedLedTask, ( const char * ) "Red", 128, NULL, mainCHECK_TASK_PRIORITY, NULL );

	vTaskStartScheduler();
}

/*================================ private ==================================*/

