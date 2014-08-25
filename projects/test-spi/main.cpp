/*
 * Copyright 2013 OpenMote Technologies, S.L.
 */

/**
 *
 * @file       test-spi.cpp
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

/*================================ define ===================================*/

#define GREEN_LED_TASK_PRIORITY             ( tskIDLE_PRIORITY + 1 )
#define SPI_TASK_PRIORITY                   ( tskIDLE_PRIORITY + 0 )

/*================================ typedef ==================================*/

/*=============================== variables =================================*/

uint8_t spi_buffer[] = {'O','p','e','n','M','o','t','e','-','C','C','2','5','3','8'};
uint8_t* spi_ptr = spi_buffer;
uint8_t  spi_len = sizeof(spi_buffer);

/*=============================== prototypes ================================*/

static void prvGreenLedTask(void *pvParameters);
static void prvSpiTask(void *pvParameters);

/*================================= public ==================================*/

int main (void)
{
    // Set the TPS62730 in bypass mode (Vin = 3.3V, Iq < 1 uA)
    tps62730.setBypass();

    spi.enable(SPI_MODE, SPI_PROTOCOL, SPI_DATAWIDTH, SPI_BAUDRATE);

    // Create two FreeRTOS tasks
    xTaskCreate(prvGreenLedTask, (const char *) "Green", 128, NULL, GREEN_LED_TASK_PRIORITY, NULL);
    xTaskCreate(prvSpiTask, (const char *) "Spi", 128, NULL, SPI_TASK_PRIORITY, NULL);

    // Kick the FreeRTOS scheduler
    vTaskStartScheduler();
}

static void prvSpiTask(void *pvParameters)
{
    while (true)
    {
        led_red.on();
        spi.writeByte(spi_ptr, spi_len);
        led_red.off();
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

/*================================ private ==================================*/
