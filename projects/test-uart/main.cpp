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

#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"

#include "openmote-cc2538.h"

#include "Serial.h"

/*================================ define ===================================*/

#define GREEN_LED_TASK_PRIORITY             ( tskIDLE_PRIORITY + 1 )
#define SERIAL_TASK_PRIORITY                ( tskIDLE_PRIORITY + 0 )

/*================================ typedef ==================================*/

/*=============================== prototypes ================================*/

static void prvGreenLedTask(void *pvParameters);
static void prvSerialTask(void *pvParameters);

/*=============================== variables =================================*/

uint8_t serial_buffer[] = {'O','p','e','n','M','o','t','e','-','C','C','2','5','3','8', '\r', '\n'};
uint8_t* serial_ptr = serial_buffer;
uint8_t serial_len  = sizeof(serial_buffer);

static Serial serial(uart);

/*================================= public ==================================*/

int main (void)
{
    // Set the TPS62730 in bypass mode (Vin = 3.3V, Iq < 1 uA)
    tps62730.setBypass();

    // Enable the UART peripheral and the serial driver
    uart.enable(UART_BAUDRATE, UART_CONFIG, UART_INT_MODE);
    serial.enable();

    // Create two FreeRTOS tasks
    xTaskCreate(prvGreenLedTask, (const char *) "Green", 128, NULL, GREEN_LED_TASK_PRIORITY, NULL);
    xTaskCreate(prvSerialTask, (const char *) "Serial", 128, NULL, SERIAL_TASK_PRIORITY, NULL);

    // Kick the FreeRTOS scheduler
    vTaskStartScheduler();
}

/*=============================== protected =================================*/

/*================================ private ==================================*/

static void prvSerialTask(void *pvParameters)
{
    // Forever
    while (true)
    {
        // Turn on red LED
        led_red.on();

        // Print buffer via Serial/UART
        serial.printf(serial_ptr, serial_len);

        // Turn off red LED
        led_red.off();

        // Delay for 250 ms
        vTaskDelay(250 / portTICK_RATE_MS);
    }
}

static void prvGreenLedTask(void *pvParameters)
{
    // Forever
    while (true)
    {
        // Turn off green LED for 950 ms
        led_green.off();
        vTaskDelay(950 / portTICK_RATE_MS);

        // Turn on green LED for 50 ms
        led_green.on();
        vTaskDelay(50 / portTICK_RATE_MS);
    }
}

