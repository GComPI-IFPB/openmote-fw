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

#include <string.h>

#include "openmote-cc2538.h"

#include "Board.h"
#include "Gpio.h"
#include "I2c.h"
#include "Radio.h"
#include "Serial.h"

#include "Adxl346.h"
#include "Tps62730.h"

#include "Callback.h"
#include "Scheduler.h"
#include "Semaphore.h"
#include "Task.h"

/*================================ define ===================================*/

#define GREEN_LED_TASK_PRIORITY             ( tskIDLE_PRIORITY + 0 )
#define SENSOR_TASK_PRIORITY                ( tskIDLE_PRIORITY + 1 )
#define CONCENTRATOR_TASK_PRIORITY          ( tskIDLE_PRIORITY + 1 )

#define RADIO_CHANNEL                       ( 26 )
#define RADIO_POWER                         ( 0xFF )

#ifdef I2C_BAUDRATE
#undef I2C_BAUDRATE
#endif
#define I2C_BAUDRATE                        ( 400000 )

#ifdef UART_BAUDRATE
#undef UART_BAUDRATE
#endif
#define UART_BAUDRATE                       ( 576000 )

#define SAMPLES_PER_READ					( 6 )
#define SAMPLES_PER_PACKET					( 120 )

/*================================ typedef ==================================*/

/*=============================== prototypes ================================*/

static void prvGreenLedTask(void *pvParameters);
static void prvSensorTask(void *pvParameters);
static void prvConcentratorTask(void *pvParameters);

static void radioRxInitCallback(void);
static void radioRxDoneCallback(void);
static void radioTxInitCallback(void);
static void radioTxDoneCallback(void);
static void adxl346Callback(void);

/*=============================== variables =================================*/

static Serial serial(uart);

static SemaphoreBinary rxSemaphore, txSemaphore;
static SemaphoreBinary adxl346Semaphore(false);

static PlainCallback radioRxInitCallback_{radioRxInitCallback};
static PlainCallback radioRxDoneCallback_{radioRxDoneCallback};
static PlainCallback radioTxInitCallback_{radioTxInitCallback};
static PlainCallback radioTxDoneCallback_{radioTxDoneCallback};

static PlainCallback adxl346Callback_{adxl346Callback};

static uint8_t  radioBuffer[128];
static uint8_t* radioBuffer_ptr;
static uint8_t  radioBuffer_len;
static int8_t  rssi;
static uint8_t lqi;
static uint8_t crc;

/*================================= public ==================================*/

int main(void) {
    // Set the TPS62730 in bypass mode (Vin = 3.3V, Iq < 1 uA)
    tps62730.setBypass();

    // Create FreeRTOS tasks
    xTaskCreate(prvGreenLedTask, (const char *) "LedTask", 128, NULL, GREEN_LED_TASK_PRIORITY, NULL);
    xTaskCreate(prvSensorTask, (const char *) "Sensor", 128, NULL, SENSOR_TASK_PRIORITY, NULL);
    // xTaskCreate(prvConcentratorTask, (const char *) "Concentrator", 128, NULL, CONCENTRATOR_TASK_PRIORITY, NULL);

    // Start the scheduler
    Scheduler::run();
}

/*================================ private ==================================*/

static void prvGreenLedTask(void *pvParameters) {
    // Forever
    while (true) {
        // Turn off the green LED and keep it for 950 ms
        led_green.off();
        Task::delay(950);

        // Turn on the green LED and keep it for 50 ms
        led_green.on();
        Task::delay(50);
    }
}

static void prvSensorTask(void *pvParameters) {
    uint8_t buffer[6];
    uint8_t counter;

    // Enable the I2C bus at 400 kHz
    i2c.enable(I2C_BAUDRATE);

    // Set the ADXL346 callback
    adxl346.setCallback(&adxl346Callback_);

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
    // adxl346.calibrate();

    // Forever
    while (true) { 
        // Restore pointer and counter
        radioBuffer_ptr = radioBuffer;
        counter = 0;

        // Wait until packet is complete
        while (counter < SAMPLES_PER_PACKET) {
            // Wait until ADXL346 samples are available
            if (adxl346Semaphore.take()) {
                // Read number of samples available
                uint8_t samples = adxl346.samplesAvailable();
                
                // Read samples for each axis
                adxl346.readSamples(buffer, samples);

                // We expect 6 samples per read
                if (samples == SAMPLES_PER_READ) {
                    // Copy samples to radio packet
                    memcpy(radioBuffer_ptr, buffer, samples);
                    
                    // Update pointer and counter
                    radioBuffer_ptr += samples;
                    counter += samples;
                }
            }
        }

        // Wait until radio is available
        if (txSemaphore.take()) {
            // Turn radio on, load packet and fire
            radio.on();
            radio.loadPacket(radioBuffer, counter);
            radio.transmit();
        }
    }
}

static void prvConcentratorTask(void *pvParameters) {
    RadioResult result;

    // Enable the UART peripheral
    uart.enable(UART_BAUDRATE);

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
        if (rxSemaphore.take()) {
            // Get packet from the radio
            radioBuffer_ptr = radioBuffer;
            radioBuffer_len = sizeof(radioBuffer);
            result = radio.getPacket(radioBuffer_ptr, &radioBuffer_len, &rssi, &lqi, &crc);

            // Check CRC and packet length
            if (result == RadioResult_Success) {
                // Send buffer over serial
                serial.write(radioBuffer_ptr, radioBuffer_len);
            }

            // Turn off the radio
            radio.off();
        }
    }
}

static void adxl346Callback(void) {
    adxl346Semaphore.giveFromInterrupt();
}

static void radioTxInitCallback(void) {
    led_red.on();
}

static void radioTxDoneCallback(void) {
    led_red.off();
    txSemaphore.giveFromInterrupt();
}

static void radioRxInitCallback(void) {
    led_red.on();
}

static void radioRxDoneCallback(void) {
    led_red.off();
    rxSemaphore.giveFromInterrupt();
}
