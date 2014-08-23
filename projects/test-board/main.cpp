/*
 * Copyright 2013 OpenMote Technologies, S.L.
 */

/**
 *
 * @file       test-board.cpp
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

#include "Callback.h"

/*================================ define ===================================*/

#define RADIO_MODE_RX                       ( 0 )
#define RADIO_MODE_TX                       ( 1 )
#define RADIO_MODE                          ( RADIO_MODE_TX )

#define PAYLOAD_LENGTH                       ( 124 )

#define GREEN_LED_TASK_PRIORITY             ( tskIDLE_PRIORITY + 2 )
#define BUTTON_TASK_PRIORITY                ( tskIDLE_PRIORITY + 1 )
#define RADIO_RX_TASK_PRIORITY              ( tskIDLE_PRIORITY + 0 )
#define RADIO_TX_TASK_PRIORITY              ( tskIDLE_PRIORITY + 0 )

/*================================ typedef ==================================*/

/*=============================== variables =================================*/

static xSemaphoreHandle rxSemaphore;
static xSemaphoreHandle txSemaphore;
static xSemaphoreHandle buttonSempahore;

static uint8_t buffer[PAYLOAD_LENGTH];
static int8_t rssi;
static uint8_t crc;

/*=============================== prototypes ================================*/

static void prvGreenLedTask(void *pvParameters);
static void prvButtonTask(void *pvParameters);
static void prvRadioRxTask(void *pvParameters);
static void prvRadioTxTask(void *pvParameters);

static void buttonCallback(void);
static void rxInit(void);
static void rxDone(void);
static void txInit(void);
static void txDone(void);

GenericCallback rxInitCallback(&rxInit);
GenericCallback rxDoneCallback(&rxDone);
GenericCallback txInitCallback(&txInit);
GenericCallback txDoneCallback(&txDone);
GenericCallback userCallback(buttonCallback);

/*================================= public ==================================*/

int main (void)
{
    // Set the TPS62730 in bypass mode (Vin = 3.3V, Iq < 1 uA)
    tps62730.setBypass();

    // Enable the UART interface
    uart.enable(UART_BAUDRATE, UART_CONFIG, UART_INT_MODE);

    // Configure the IEEE 802.15.4 radio
    radio.setTxCallbacks(&txInitCallback, &txDoneCallback);
    radio.setRxCallbacks(&rxInitCallback, &rxDoneCallback);
    radio.enable();

    // Create three FreeRTOS tasks
    xTaskCreate(prvGreenLedTask, (const char *) "Green", 128, NULL, GREEN_LED_TASK_PRIORITY, NULL);
    xTaskCreate(prvButtonTask, (const char *) "Button", 128, NULL, BUTTON_TASK_PRIORITY, NULL);
#if (RADIO_MODE == RADIO_MODE_RX)
    xTaskCreate(prvRadioRxTask, (const char *) "RadioRx", 128, NULL, RADIO_RX_TASK_PRIORITY, NULL);
#elif (RADIO_MODE == RADIO_MODE_TX)
    xTaskCreate(prvRadioTxTask, (const char *) "RadioTx", 128, NULL, RADIO_TX_TASK_PRIORITY, NULL);
#endif

    // Kick the FreeRTOS scheduler
    vTaskStartScheduler();
}

/*================================ private ==================================*/

static void prvGreenLedTask(void *pvParameters)
{
    // Forever
    while (true)
    {
        // Turn off the green LED and keep it for 950 ms
        led_green.off();
        vTaskDelay(950 / portTICK_RATE_MS);

        // Turn on the green LED and keep it for 50 ms
        led_green.on();
        vTaskDelay(50 / portTICK_RATE_MS);
    }
}

static void prvButtonTask(void *pvParameters)
{
    // Create the button semaphore
    buttonSempahore = xSemaphoreCreateMutex();

    // Configure the user button
    button_user.setCallback(&userCallback);
    button_user.enableInterrupt();

    // Forever
    while (true)
    {
        // Take the buttonSemaphore, block until available
        if (xSemaphoreTake(buttonSempahore, (TickType_t) portMAX_DELAY) == pdTRUE)
        {
            // Toggle the orange LED
            led_orange.toggle();
        }
    }
}

static void prvRadioRxTask(void *pvParameters)
{
    // Create the receive semaphore
    rxSemaphore = xSemaphoreCreateMutex();

    // Take the receive semaphore so that we block until a packet is received
    xSemaphoreTake(rxSemaphore, (TickType_t) portMAX_DELAY);

    // Turn on the radio transceiver
    radio.on();

    // Forever
    while (true)
    {
        // Put the radio transceiver in receive mode
        radio.receive();

        // Turn the yellow LED on when a the radio is receiving
        led_yellow.on();

        // Take the rxSemaphre, block until available
        if (xSemaphoreTake(rxSemaphore, (TickType_t) portMAX_DELAY) == pdTRUE)
        {
            // Turn the yellow LED off when a packet is received
            led_yellow.off();

            // Get a packet from the radio buffer
            radio.getPacket(buffer, sizeof(buffer), &rssi, &crc);

            // Transmit the RSSI byte over the UART
            uart.writeByte(rssi);
        }
    }
}

static void prvRadioTxTask(void *pvParameters)
{
    // Create the transmit semaphore
    txSemaphore = xSemaphoreCreateMutex();

    // Turn on the radio transceiver
    radio.on();

    // Forever
    while (true)
    {
        // Take the txSemaphre, block until available
        if (xSemaphoreTake(txSemaphore, (TickType_t) portMAX_DELAY) == pdTRUE)
        {
            // Turn the yellow LED on when the packet is being loaded
            led_yellow.on();

            // Put the radio transceiver in transmit mode
            radio.loadPacket(buffer, sizeof(buffer));

            // Put the radio transceiver in transmit mode
            radio.transmit();

            // Turn the yellow LED off when the packet has beed loaded
            led_yellow.off();

            // Delay the transmission of the next packet 100 ms
            vTaskDelay(100 / portTICK_RATE_MS);
        }
    }
}

static void buttonCallback(void)
{
    // Determines if the interrupt triggers a context switch
    static BaseType_t xHigherPriorityTaskWoken;
    xHigherPriorityTaskWoken = pdFALSE;

    // Give the button semaphore as the button has been pressed
    xSemaphoreGiveFromISR(buttonSempahore, &xHigherPriorityTaskWoken);

    // Force a context switch after the interrupt if required
    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}

static void rxInit(void)
{
    // Turn on the radio LED as the radio is now receiving a packet
    led_red.on();
}

static void rxDone(void)
{
    // Determines if the interrupt triggers a context switch
    static BaseType_t xHigherPriorityTaskWoken;
    xHigherPriorityTaskWoken = pdFALSE;

    // Turn off the radio LED as the packet is now received
    led_red.off();

    // Give the receive semaphore as the packet has been received
    xSemaphoreGiveFromISR(rxSemaphore, &xHigherPriorityTaskWoken);

    // Force a context switch after the interrupt if required
    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}

static void txInit(void)
{
    // Turn on the radio LED as the packet is now transmitting
    led_red.on();
}

static void txDone(void)
{
    // Determines if the interrupt triggers a context switch
    static BaseType_t xHigherPriorityTaskWoken;
    xHigherPriorityTaskWoken = pdFALSE;

    // Turn off the radio LED as the packet is transmitted
    led_red.off();

    // Give the transmit semaphore as the packet has been transmitted
    xSemaphoreGiveFromISR(txSemaphore, &xHigherPriorityTaskWoken);

    // Force a context switch after the interrupt if required
    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}
