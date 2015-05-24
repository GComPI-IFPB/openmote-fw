/**
 * @file       Hdlc.cpp
 * @author     Pere Tuset-Peiro (peretuset@openmote.com)
 * @version    v0.1
 * @date       May, 2015
 * @brief
 *
 * @copyright  Copyright 2015, OpenMote Technologies, S.L.
 *             This file is licensed under the GNU General Public License v2.
 */

/*================================ include ==================================*/

#include "Hdlc.h"

/*================================ define ===================================*/

static const uint8_t HDLC_FLAG        = 0x7E;
static const uint8_t HDLC_ESCAPE      = 0x7D;
static const uint8_t HDLC_ESCAPE_MASK = 0x20;

/*================================ typedef ==================================*/

/*=============================== variables =================================*/

/*=============================== prototypes ================================*/

/*================================= public ==================================*/

Hdlc::Hdlc(CircularBuffer& rxCircularBuffer_, CircularBuffer& txCircularBuffer_):
    rxCircularBuffer(rxCircularBuffer_), txCircularBuffer(txCircularBuffer_)
{
}

void Hdlc::rxOpen(void)
{
    rxStatus = HdlcStatus_Idle;
    rxLastByte = 0;
    rxIsEscaping = false;
    rxCrc.init();
}

HdlcStatus Hdlc::rxPut(uint8_t byte)
{
    // Check the current HDLC status
    if (rxStatus == HdlcStatus_Idle &&
        rxLastByte == HDLC_FLAG &&
        byte != HDLC_FLAG) // Start of HDLC frame
    {
        rxStatus = HdlcStatus_Busy;

        // Parse the current byte
        rxParse(byte);
    }
    else if (rxStatus == HdlcStatus_Busy &&
             byte != HDLC_FLAG) // Middle of HDLC frame
    {
        // Parse the current byte
        rxParse(byte);
    }
    else if (rxStatus == HdlcStatus_Busy &&
             byte == HDLC_FLAG) // End of HDLC frame
    {
        // Finished receiving an HDLC frame
        rxStatus = HdlcStatus_Done;
    }

    // Record the last received byte
    rxLastByte = byte;

    // Return the current status
    return rxStatus;
}

bool Hdlc::rxClose(void)
{
    // Return the status of the receive CRC
    return rxCrc.check();
}

void Hdlc::txOpen(void)
{
    // Initialize the transmit CRC module
    txCrc.init();

    // Write the opening HDLC flag to the transmit buffer
    txCircularBuffer.write(HDLC_FLAG);
}

void Hdlc::txPut(uint8_t byte)
{
    // Push the byte to the transmit CRC module
    txCrc.set(byte);

    // Check if we are transmitting and HDLF flag or escape byte
    if (byte == HDLC_FLAG || byte == HDLC_ESCAPE)
    {
        // If so, write an HDLC escape symbol to the transmit buffer
        txCircularBuffer.write(HDLC_ESCAPE);

        // Transform the current byte
        byte ^= HDLC_ESCAPE_MASK;
    }

    // Write the current byte to the transmit buffer
    txCircularBuffer.write(byte);
}

void Hdlc::txClose(void)
{
    uint16_t crc;

    // Get the CRC value
    crc = txCrc.get();

    // Write the CRC value to the transmit buffer
    txCircularBuffer.write((crc >> 8) & 0xFF);
    txCircularBuffer.write((crc >> 0) & 0xFF);

    // Write the closing HDLC flag to the transmit buffer
    txCircularBuffer.write(HDLC_FLAG);
}

/*=============================== protected =================================*/

/*================================ private ==================================*/

void Hdlc::rxParse(uint8_t byte)
{
    // Check the received byte
    if (byte == HDLC_ESCAPE)
    {
        // Mark as currently escaping
        rxIsEscaping = true;
    }
    else
    {
        if (rxIsEscaping == true) // If we received an escape byte
        {
            // Update the received byte
            byte = byte ^ HDLC_ESCAPE_MASK;

            // Unmark as currently escaping
            rxIsEscaping = false;
        }

        // Write a byte to the receive buffer
        rxCircularBuffer.write(byte);

        // Push the byte to the CRC module
        rxCrc.set(byte);
    }
}
