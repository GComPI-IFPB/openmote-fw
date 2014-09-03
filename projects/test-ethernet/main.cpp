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

#include <string.h>

#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"

#include "openmote-cc2538.h"

/*================================ define ===================================*/

#define GREEN_LED_TASK_PRIORITY             ( tskIDLE_PRIORITY + 1 )
#define ETHERNET_TASK_PRIORITY              ( tskIDLE_PRIORITY + 0 )

/*================================ typedef ==================================*/

/*=============================== prototypes ================================*/

static void prvGreenLedTask(void *pvParameters);
static void prvEthernetTask(void *pvParameters);

/*=============================== variables =================================*/

uint8_t eui64_address[8];
uint8_t eui48_address[6];
uint8_t ethernet_payload[60] = {
                            0xA4, 0x5D, 0x36, 0x5B, 0xA4, 0xD0,
                            0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
                            0x08, 0x06,
                            0x00, 0x01, 0x08, 0x06, 0x04, 0x00, 0x02, 0x00,
                            0x00, 0x01, 0x08, 0x06, 0x04, 0x00, 0x02, 0x00,
                            0x00, 0x01, 0x08, 0x06, 0x04, 0x00, 0x02, 0x00,
                            0x00, 0x01, 0x08, 0x06, 0x04, 0x00, 0x02, 0x00,
                            0x00, 0x01, 0x08, 0x06, 0x04, 0x00, 0x02, 0x00,
                            0x00, 0x01, 0x08, 0x06, 0x04, 0x00};

/*================================= public ==================================*/

int main (void)
{
    // Set the TPS62730 in bypass mode (Vin = 3.3V, Iq < 1 uA)
    tps62730.setBypass();

    // Enable the SPI peripheral
    spi.enable(SPI_MODE, SPI_PROTOCOL, SPI_DATAWIDTH, SPI_BAUDRATE);

    // Create two FreeRTOS tasks
    xTaskCreate(prvGreenLedTask, (const char *) "Green", 128, NULL, GREEN_LED_TASK_PRIORITY, NULL);
    xTaskCreate(prvEthernetTask, (const char *) "Ethernet", 128, NULL, ETHERNET_TASK_PRIORITY, NULL);

    // Kick the FreeRTOS scheduler
    vTaskStartScheduler();
}

/*=============================== protected =================================*/

/*================================ private ==================================*/

static void prvEthernetTask(void *pvParameters)
{
    // Get the EUI-64 address
    board.getAddress(eui64_address);

    // Set the EUI-48 address
    memcpy(&eui48_address[0], &eui64_address[2], 6);

    // Set the EUI-48 address to the packet
    memcpy(&ethernet_payload[6], eui48_address, 6);

    // Use the EUI-48 address as the MAC address
    enc28j60.init(eui48_address);

    // Forever
    while (true)
    {
        // Transmit a packet every second
        vTaskDelay(1000 / portTICK_RATE_MS);

        // Turn on red LED
        led_red.on();

        // Send Ethernet payload
        enc28j60.sendPacket(ethernet_payload, sizeof(ethernet_payload));

        // Turn off red LED
        led_red.off();
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
