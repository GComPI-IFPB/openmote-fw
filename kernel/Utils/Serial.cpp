/*
 * Copyright 2013 OpenMote Technologies, S.L.
 */

/**
 *
 * @file       Serial.cpp
 * @author     Pere Tuset-Peiro (peretuset@openmote.com)
 * @version    v0.1
 * @date       May, 2014
 * @brief
 * @ingroup
 *
 */

/*================================ include ==================================*/

#include "Serial.h"

/*================================ define ===================================*/

/*================================ typedef ==================================*/

/*=============================== variables =================================*/

static const uint8_t CRC_LENGTH = 2;    // Length of the CRC

/*=============================== prototypes ================================*/

/*================================= public ==================================*/

Serial::Serial(UartDriver& uart_):
    uart(uart_), \
    rxBuffer(receive_buffer, sizeof(receive_buffer)), \
    txBuffer(transmit_buffer, sizeof(transmit_buffer)), \
    hdlc(rxBuffer, txBuffer), \
    rxCallback(this, &Serial::rxCallback_), txCallback(this, &Serial::txCallback_)
{
}

void Serial::init(void)
{
    // Register UART callbacks
    uart.setRxCallback(&rxCallback);
    uart.setTxCallback(&txCallback);

    // Enable UART interrupts
    uart.enableInterrupt();

    // Open the HDLC receive buffer
    hdlc.rxOpen();

    // Lock the UART receive
    uart.rxLock();
}

void Serial::printf(uint8_t* data, uint32_t size)
{
    uint8_t byte;

    // Take the UART lock
    uart.txLock();

    // Open the HDLC transmit buffer
    hdlc.txOpen();

    // For each byte in the buffer
    while(size--)
    {
        // Put the byte in the HDLC transmit buffer
        hdlc.txPut(*data++);
    }

    // Close the HDLC buffer
    hdlc.txClose();

    // Read byte from the UART transmit buffer
    txBuffer.read(&byte);

    // Write byte to the UART
    uart.writeByte(byte);
}

uint32_t Serial::scanf(uint8_t* buffer, uint32_t size)
{
    uint8_t data;
    uint32_t length;

    // Lock the UART receive
    uart.rxLock();

    // Update the length value and account for the CRC bytes
    length = rxBuffer.getSize() - CRC_LENGTH;

    // Check for buffer overflow
    if (length <= size)
    {
        // Copy all bytes to the buffer except the CRC bytes
        while (size--) {
            rxBuffer.read(&data);
            *buffer++ = data;
        }

        // Read the CRC bytes from the buffer
        rxBuffer.read(&data);
        rxBuffer.read(&data);
    }
    else
    {
        length = 0;
    }

    // Open the HDLC receive buffer
    hdlc.rxOpen();

    return length;
}

/*=============================== protected =================================*/

/*================================ private ==================================*/

void Serial::rxCallback_(void)
{
    uint8_t byte;
    HdlcStatus hdlcStatus;
    bool status;

    // Read byte from the UART
    byte = uart.readByte();

    // Put the byte in the HDLC receive buffer
    hdlcStatus = hdlc.rxPut(byte);

    if (hdlcStatus == HdlcStatus_Done) // If HDLC frame is completed
    {
        // Close the HDLC frame
        status = hdlc.rxClose();

        if (status) // CRC valid
        {
            uart.rxUnlockFromInterrupt();
        }
        else // CRC NOT valid
        {
            // Reset the receive buffer
            rxBuffer.reset();

            // Open the HDLC receive buffer
            hdlc.rxOpen();
        }
    }
}

void Serial::txCallback_(void)
{
    uint8_t byte;

    // Read byte from the UART transmit buffer
    if (txBuffer.read(&byte) == 0)
    {
        // Write byte to the UART
        uart.writeByte(byte);
    }
    else
    {
        // Once done, free the UART lock
        uart.txUnlockFromInterrupt();
    }
}
