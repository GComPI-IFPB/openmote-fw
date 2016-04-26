/**
 * @file       main.cpp
 * @author     Pere Tuset-Peiro (peretuset@openmote.com)
 * @version    v0.1
 * @date       May, 2015
 * @brief
 *
 * @copyright  Copyright 2015, OpenMote Technologies, S.L.
 *             This file is licensed under the GNU General Public License v2.
 */

/*================================ include ==================================*/

#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"

#include "openmote-cc2538.h"

#include "Board.h"
#include "Callback.h"
#include "Mutex.h"
#include "Radio.h"
#include "Adxl346.h"
#include "Serial.h"

/*================================ define ===================================*/

#define GREEN_LED_TASK_PRIORITY             ( tskIDLE_PRIORITY + 0 )
#define SENSOR_TASK_PRIORITY                ( tskIDLE_PRIORITY + 1 )
#define CONCENTRATOR_TASK_PRIORITY          ( tskIDLE_PRIORITY + 1 )

#define RADIO_CHANNEL                       ( 26 )
#define RADIO_POWER                         ( 0xFF )

#ifdef I2C_BAUDRATE
#undef I2C_BAUDRATE
#define I2C_BAUDRATE                        ( 400000 )
#endif

#ifdef UART_BAUDRATE
#undef UART_BAUDRATE
#define UART_BAUDRATE                       ( 576000 )
#endif

/*================================ typedef ==================================*/

/*=============================== prototypes ================================*/

static void prvGreenLedTask(void *pvParameters);
static void prvSensorTask(void *pvParameters);
static void prvConcentratorTask(void *pvParameters);

static void radioRxInitCallback(void);
static void radioRxDoneCallback(void);
static void radioTxInitCallback(void);
static void radioTxDoneCallback(void);

/*=============================== variables =================================*/

Serial serial(uart);

Mutex txMutex, rxMutex;

PlainCallback radioRxInitCallback_{radioRxInitCallback};
PlainCallback radioRxDoneCallback_{radioRxDoneCallback};
PlainCallback radioTxInitCallback_{radioTxInitCallback};
PlainCallback radioTxDoneCallback_{radioTxDoneCallback};

uint8_t  radioBuffer[128];
uint8_t* radioBuffer_ptr;
uint8_t  radioBuffer_len;
int8_t  rssi;
uint8_t lqi;
uint8_t crc;

/*================================= public ==================================*/

int main(void) {
    // Set the TPS62730 in bypass mode (Vin = 3.3V, Iq < 1 uA)
    tps62730.setBypass();

    // Create FreeRTOS tasks
    xTaskCreate(prvGreenLedTask, (const char *) "LedTask", 128, NULL, GREEN_LED_TASK_PRIORITY, NULL);
    // xTaskCreate(prvSensorTask, (const char *) "Sensor", 128, NULL, SENSOR_TASK_PRIORITY, NULL);
    xTaskCreate(prvConcentratorTask, (const char *) "Concentrator", 128, NULL, CONCENTRATOR_TASK_PRIORITY, NULL);

    // Kick the FreeRTOS scheduler
    vTaskStartScheduler();
}

/*================================ private ==================================*/

static void prvGreenLedTask(void *pvParameters) {
    // Forever
    while (true) {
        // Turn off the green LED and keep it for 950 ms
        led_green.off();
        vTaskDelay(950 / portTICK_RATE_MS);

        // Turn on the green LED and keep it for 50 ms
        led_green.on();
        vTaskDelay(50 / portTICK_RATE_MS);
    }
}

static void prvSensorTask(void *pvParameters) {
    static uint16_t x, y, z;
    uint8_t data[6];

    // Enable the I2C bus at 400 kHz
    i2c.enable(I2C_BAUDRATE);

    // Enable the ADXL346
    adxl346.enable();

    // Enable the radio
    radio.enable();

    // Set the radio channel and power
    radio.setChannel(RADIO_CHANNEL);
    radio.setPower(RADIO_POWER);

    // Set Radio receive callbacks
    radio.setTxCallbacks(&radioTxInitCallback_, &radioTxDoneCallback_);
    radio.enableInterrupts();

    // Calibrate the ADXL346 sensor
    adxl346.calibrate();

    // Forever
    while (true) { 
        // Read x, y and z samples from the ADXL346
        if (adxl346.readSample(&x, &y, &z)) {
            if (txMutex.take()) {
                // Copy samples to buffer
                data[0] = (x >> 8) & 0xFF;
                data[1] = (x >> 0) & 0xFF;
                data[2] = (y >> 8) & 0xFF;
                data[3] = (y >> 0) & 0xFF;
                data[4] = (z >> 8) & 0xFF;
                data[5] = (z >> 0) & 0xFF;

                // Turn radio on, load packet and fire
                radio.on();
                radio.loadPacket(data, sizeof(data));
                radio.transmit();
            }
            vTaskDelay(5 / portTICK_RATE_MS);
        }
    }
}

static void prvConcentratorTask(void *pvParameters) {
    RadioResult result;

    // Enable the UART peripheral
    uart.enable(UART_BAUDRATE, UART_CONFIG, UART_INT_MODE);

    // Enable Radio module
    radio.enable();

    // Set the radio channel
    radio.setChannel(RADIO_CHANNEL);

    // Set Radio receive callbacks
    radio.setRxCallbacks(&radioRxInitCallback_, &radioRxDoneCallback_);
    radio.enableInterrupts();

    // Init the serial
    serial.init();

    // Forever
    while (true) {       
        // Start receiving
        radio.on();
        radio.receive();

        // This call blocks until a radio frame is received
        if (rxMutex.take()) {
            // Get packet from the radio
            radioBuffer_ptr = radioBuffer;
            radioBuffer_len = sizeof(radioBuffer);
            result = radio.getPacket(radioBuffer_ptr, &radioBuffer_len, &rssi, &lqi, &crc);

            // Check CRC and packet length
            if (result == RadioResult_Success && radioBuffer_len == 6) {
                // Turn off the radio
                radio.off();

                // Send buffer over serial
                serial.write(radioBuffer_ptr, 6);
            }
        }
    }
}

static void radioTxInitCallback(void) {
    led_red.on();
}

static void radioTxDoneCallback(void) {
    led_red.off();
    txMutex.giveFromInterrupt();
}

static void radioRxInitCallback(void) {
    led_red.on();
}

static void radioRxDoneCallback(void) {
    led_red.off();
    rxMutex.giveFromInterrupt();
}