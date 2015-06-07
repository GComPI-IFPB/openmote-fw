/**
 * @file       SnifferEthernet.cpp
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

#include "SnifferEthernet.h"

#include "openmote-cc2538.h"

/*================================ define ===================================*/

/*================================ typedef ==================================*/

/*=============================== prototypes ================================*/

/*=============================== variables =================================*/

const uint8_t SnifferEthernet::broadcastAddress[6] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
const uint8_t SnifferEthernet::ethernetType[2]     = {0x80, 0x9A};

/*================================= public ==================================*/

SnifferEthernet::SnifferEthernet(Board& board, Radio& radio, Ethernet& ethernet):
    SnifferCommon(board, radio), ethernet_(ethernet), \
    ethernetBuffer_ptr(ethernetBuffer), ethernetBuffer_len(sizeof(ethernetBuffer))
{
}

void SnifferEthernet::init(void)
{
    // Get the EUI48
    board_.getEUI48(macAddress);

    // Initialize the Ethernet with EUI48
    ethernet_.init(macAddress);

    // Call common class
    SnifferCommon::init();
}

void SnifferEthernet::processRadioFrame(void)
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

            // Initialize Ethernet frame
            ethernetBuffer_ptr = ethernetBuffer;
            ethernetBuffer_len = sizeof(ethernetBuffer);
            initEthernetFrame(radioBuffer_ptr, radioBuffer_len, rssi, lqi, crc);

            // Transmit the radio frame over Ethernet
            ethernet_.transmitFrame(ethernetBuffer_ptr, ethernetBuffer_len);
        }
    }
}

/*================================ private ==================================*/

void SnifferEthernet::initEthernetFrame(uint8_t* buffer, uint8_t length, int8_t rssi, uint8_t lqi, uint8_t crc)
{
    // Pre-calculate the frame length
    uint32_t frameLength = 6 + 2 + 6 + length + 2;

    // Check that we do not overflow the Ethernet buffer
    if (frameLength > ethernetBuffer_len)
    {
        return;
    }

    // Reset the Ethernet buffer length
    ethernetBuffer_len = 0;

    // Set MAC destination address
    memcpy(&ethernetBuffer[0], SnifferEthernet::broadcastAddress, 6);
    ethernetBuffer_len += 6;

    // Set MAC source address
    memcpy(&ethernetBuffer[6], &macAddress, 6);
    ethernetBuffer_len += 6;

    // Set MAC type
    memcpy(&ethernetBuffer[12], SnifferEthernet::ethernetType, 2);
    ethernetBuffer_len += 2;

    // Need to set the PHR field?
    // memset(&ethernetBuffer[14], length, 1);
    // ethernetBuffer_len += 1;

    // Copy the IEEE 802.15.4 payload
    memcpy(&ethernetBuffer[14], buffer, length);
    ethernetBuffer_len += length;

    // Ensure that we meet the minimum Ethernet frame size
    if (frameLength < 60)
    {
        // Calculate the remaining space
        uint32_t bytes = 60 - frameLength;

        // Fill the remaining space with zeros
        memset(&ethernetBuffer[ethernetBuffer_len], 0x00, bytes);
        ethernetBuffer_len += bytes;
    }

    // Copy the IEEE 802.15.4 RSSI
    ethernetBuffer[ethernetBuffer_len] = rssi;
    ethernetBuffer_len += 1;

    // Copy the IEEE 802.15.4 CRC and LQI
    ethernetBuffer[ethernetBuffer_len] = crc | lqi;
    ethernetBuffer_len += 1;
}
