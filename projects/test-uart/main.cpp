/*
 * Copyright 2013 OpenMote Technologies, S.L.
 */

/**
 *
 * @file       test-uart.cpp
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

#include "string.h"

/*================================ define ===================================*/

#define GREEN_LED_TASK_PRIORITY             ( tskIDLE_PRIORITY + 1 )
#define SERIAL_TASK_A_PRIORITY              ( tskIDLE_PRIORITY + 0 )
#define SERIAL_TASK_B_PRIORITY              ( tskIDLE_PRIORITY + 2 )

/*================================ typedef ==================================*/

/*=============================== variables =================================*/

static uint8_t serialDataA[64];
static uint8_t serialDataB[64];

static Serial serial(uart);

/*=============================== prototypes ================================*/

static void prvGreenLedTask(void *pvParameters);
static void prvSerialTaskA(void *pvParameters);
static void prvSerialTaskB(void *pvParameters);

/*================================= public ==================================*/

int main (void)
{
    // Set the TPS62730 in bypass mode (Vin = 3.3V, Iq < 1 uA)
    tps62730.setBypass();

    uart.enable(UART_BAUDRATE, UART_CONFIG, UART_INT_MODE);
    serial.enable();

    memset(serialDataA, 0xAA, sizeof(serialDataA));
    memset(serialDataB, 0xBB, sizeof(serialDataB));

    // Create two FreeRTOS tasks
    xTaskCreate(prvGreenLedTask, (const char *) "Green", 128, NULL, GREEN_LED_TASK_PRIORITY, NULL);
    xTaskCreate(prvSerialTaskA, (const char *) "Serial", 128, NULL, SERIAL_TASK_A_PRIORITY, NULL);
    xTaskCreate(prvSerialTaskB, (const char *) "Serial", 128, NULL, SERIAL_TASK_B_PRIORITY, NULL);

    // Kick the FreeRTOS scheduler
    vTaskStartScheduler();
}

/*=============================== protected =================================*/

/*================================ private ==================================*/

static void prvSerialTaskA(void *pvParameters)
{
    while(true)
    {
        led_red.on();
        serial.printf(serialDataA, sizeof(serialDataA));
        led_red.off();
        vTaskDelay(100 / portTICK_RATE_MS);
    }
}

static void prvSerialTaskB(void *pvParameters)
{
    while(true)
    {
        led_orange.on();
        serial.printf(serialDataB, sizeof(serialDataB));
        led_orange.off();
        vTaskDelay(100 / portTICK_RATE_MS);
    }
}

static void prvGreenLedTask(void *pvParameters)
{
    while(true)
    {
        led_green.off();
        vTaskDelay(950 / portTICK_RATE_MS);
        led_green.on();
        vTaskDelay(50 / portTICK_RATE_MS);
    }
}
