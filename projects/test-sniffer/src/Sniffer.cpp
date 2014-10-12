/*
 * Copyright 2013 OpenMote Technologies, S.L.
 */

/**
 *
 * @file       Sniffer.cpp
 * @author     Pere Tuset-Peiro (peretuset@openmote.com)
 * @version    v0.1
 * @date       May, 2014
 * @brief
 * @ingroup
 *
 */

/*================================ include ==================================*/

#include "Sniffer.h"
#include "Board.h"

#include "string.h"

#include "openmote-cc2538.h"

/*================================ define ===================================*/

/*================================ typedef ==================================*/

/*=============================== prototypes ================================*/

/*=============================== variables =================================*/

const uint8_t Sniffer::broadcastAddress[6] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
const uint8_t Sniffer::ethernetType[2]     = {0x80, 0x9A};

/*================================= public ==================================*/

Sniffer::Sniffer(Board& board, Ethernet& ethernet, Radio& radio):
    board_(board), ethernet_(ethernet), radio_(radio), \
    snifferRadioRxInitCallback_(this, &Sniffer::radioRxInitCallback), \
    snifferRadioRxDoneCallback_(this, &Sniffer::radioRxDoneCallback), \
    radioBuffer_ptr(radioBuffer), radioBuffer_len(sizeof(radioBuffer)), \
    ethernetBuffer_ptr(ethernetBuffer), ethernetBuffer_len(sizeof(ethernetBuffer))
{
}

void Sniffer::init(void)
{
    // Create the receive semaphore
    radioRxSemaphore = xSemaphoreCreateMutex();

    // Take the receive semaphore so that we block until a packet is received
    xSemaphoreTake(radioRxSemaphore, (TickType_t) portMAX_DELAY);

    // Get the EUI48
    board_.getEUI48(macAddress);

    // Initialize the Ethernet with EUI48
    ethernet_.init(macAddress);

    // Set Radio receive callbacks
    radio_.setRxCallbacks(&snifferRadioRxInitCallback_, \
                          &snifferRadioRxDoneCallback_);

    // Enable Radio module
    radio_.enable();
    radio_.enableInterrupts();
}

void Sniffer::start(void)
{
    // Start receiving
    radio_.on();
    radio_.receive();
}

void Sniffer::stop(void)
{
    // Stop receiving
    radio_.off();
}

void Sniffer::setChannel(uint8_t channel)
{
    // Set the radio channel
    radio_.setChannel(channel);
}

void Sniffer::processRadioFrame(void)
{
    static RadioResult result;

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

void Sniffer::initEthernetFrame(uint8_t* buffer, uint8_t length, int8_t rssi, uint8_t lqi, uint8_t crc)
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
    memcpy(&ethernetBuffer[0], Sniffer::broadcastAddress, 6);
    ethernetBuffer_len += 6;

    // Set MAC source address
    memcpy(&ethernetBuffer[6], &macAddress, 6);
    ethernetBuffer_len += 6;

    // Set MAC type
    memcpy(&ethernetBuffer[12], Sniffer::ethernetType, 2);
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

void Sniffer::radioRxInitCallback(void)
{
    led_red.on();
}

void Sniffer::radioRxDoneCallback(void)
{
    // Determines if the interrupt triggers a context switch
    xHigherPriorityTaskWoken = pdFALSE;

    led_red.off();

    // Give the receive semaphore as the packet has been received
    xSemaphoreGiveFromISR(radioRxSemaphore, &xHigherPriorityTaskWoken);

    // Force a context switch after the interrupt if required
    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}
