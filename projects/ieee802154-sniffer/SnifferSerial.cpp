/**
 * @file       SnifferSerial.cpp
 * @author     Pere Tuset-Peiro (peretuset@openmote.com)
 * @version    v0.1
 * @date       May, 2015
 * @brief
 *
 * @copyright  Copyright 2015, OpenMote Technologies, S.L.
 *             This file is licensed under the GNU General Public License v2.
 */

/*================================ include ==================================*/

#include <string.h>

#include "SnifferSerial.h"

#include "openmote-cc2538.h"

/*================================ define ===================================*/

/*================================ typedef ==================================*/

/*=============================== prototypes ================================*/

/*=============================== variables =================================*/

/*================================= public ==================================*/

SnifferSerial::SnifferSerial(Board& board, Radio& radio, Serial& serial):
    SnifferCommon(board, radio), serial_(serial), \
    serialBuffer_ptr(serialBuffer), serialBuffer_len(sizeof(serialBuffer))
{
}

void SnifferSerial::init(void)
{
    // Call common class
    SnifferCommon::init();
}

void SnifferSerial::processRadioFrame(void)
{
    RadioResult result;

    // This call blocks until a radio frame is received
    if (xSemaphoreTake(radioRxSemaphore, portMAX_DELAY) == pdTRUE)
    {
        // Get packet from the radio
        radioBuffer_ptr = radioBuffer;
        radioBuffer_len = sizeof(radioBuffer);
        result = radio_.getPacket(radioBuffer_ptr, &radioBuffer_len, &rssi, &lqi, &crc);

        if (result == RadioResult_Success)
        {
            // Turn off the radio
            radio_.off();
            led_orange.off();

            // Initialize Serial frame
            initSerialFrame(radioBuffer_ptr, radioBuffer_len);

            // Transmit the radio frame over Serial
            serial_.write(serialBuffer_ptr, serialBuffer_len);
        }
    }
}

/*================================ private ==================================*/

void SnifferSerial::initSerialFrame(uint8_t* buffer, uint8_t length)
{
    serialBuffer_ptr = serialBuffer;
    serialBuffer_len = sizeof(serialBuffer);

    memcpy(serialBuffer_ptr, buffer, length);

    serialBuffer_len = length;
}
