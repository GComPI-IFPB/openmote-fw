/**
 * @file       Serial.cpp
 * @author     Pere Tuset-Peiro (peretuset@openmote.com)
 * @version    v0.1
 * @date       May, 2015
 * @brief
 *
 * @copyright  Copyright 2015, OpenMote Technologies, S.L.
 *             This file is licensed under the GNU General Public License v2.
 */

/*================================ include ==================================*/

#include "Serial.h"

/*================================ define ===================================*/

/*================================ typedef ==================================*/

/*=============================== variables =================================*/

static const uint8_t CRC_LENGTH = 2;    // Length of the CRC

/*=============================== prototypes ================================*/

/*================================= public ==================================*/

Serial::Serial(UartDriver& uart):
    uart_(uart), \
    rxBuffer_(receive_buffer_, sizeof(receive_buffer_)), \
    txBuffer_(transmit_buffer_, sizeof(transmit_buffer_)), \
    hdlc_(rxBuffer_, txBuffer_), \
    rxCallback_(this, &Serial::rxCallback), txCallback_(this, &Serial::txCallback)
{
}

void Serial::init(void)
{
    // Register UART callbacks
    uart_.setRxCallback(&rxCallback_);
    uart_.setTxCallback(&txCallback_);

    // Enable UART interrupts
    uart_.enableInterrupts();

    // Open the HDLC receive buffer
    hdlc_.rxOpen();

    // Lock the UART receive
    uart_.rxLock();
}

void Serial::printf(uint8_t* data, uint32_t size)
{
    uint8_t byte;

    // Take the UART lock
    uart_.txLock();

    // Open the HDLC transmit buffer
    hdlc_.txOpen();

    // For each byte in the buffer
    while(size--)
    {
        // Put the byte in the HDLC transmit buffer
        hdlc_.txPut(*data++);
    }

    // Close the HDLC buffer
    hdlc_.txClose();

    // Read byte from the UART transmit buffer
    txBuffer_.read(&byte);

    // Write byte to the UART
    uart_.writeByte(byte);
}

uint32_t Serial::scanf(uint8_t* buffer, uint32_t size)
{
    uint8_t data;
    uint32_t length;

    // Lock the UART receive
    uart_.rxLock();

    // Update the length value and account for the CRC bytes
    length = rxBuffer_.getSize() - CRC_LENGTH;

    // Check for buffer overflow
    if (length <= size)
    {
        // Copy all bytes to the buffer except the CRC bytes
        while (size--) {
            rxBuffer_.read(&data);
            *buffer++ = data;
        }

        // Read the CRC bytes from the buffer
        rxBuffer_.read(&data);
        rxBuffer_.read(&data);
    }
    else
    {
        length = 0;
    }

    // Reset the receive buffer
    rxBuffer_.reset();

    // Open the HDLC receive buffer
    hdlc_.rxOpen();

    return length;
}

/*=============================== protected =================================*/

/*================================ private ==================================*/

void Serial::rxCallback(void)
{
    uint8_t byte;
    HdlcStatus hdlcStatus;
    bool status;

    // Read byte from the UART
    byte = uart_.readByte();

    // Put the byte in the HDLC receive buffer
    hdlcStatus = hdlc_.rxPut(byte);

    if (hdlcStatus == HdlcStatus_Done) // If HDLC frame is completed
    {
        // Close the HDLC frame
        status = hdlc_.rxClose();

        if (status) // CRC valid
        {
            uart_.rxUnlockFromInterrupt();
        }
        else // CRC NOT valid
        {
            // Reset the receive buffer
            rxBuffer_.reset();

            // Open the HDLC receive buffer
            hdlc_.rxOpen();
        }
    }
}

void Serial::txCallback(void)
{
    uint8_t byte;

    // Read byte from the UART transmit buffer
    if (txBuffer_.read(&byte) == 0)
    {
        // Write byte to the UART
        uart_.writeByte(byte);
    }
    else
    {
        // Once done, free the UART lock
        uart_.txUnlockFromInterrupt();
    }
}
