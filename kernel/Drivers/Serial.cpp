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

/*=============================== prototypes ================================*/

/*================================= public ==================================*/

Serial::Serial(UartDriver& uart_):
    uart(uart_), \
    rxBuffer(receive_buffer, sizeof(receive_buffer)), \
    txBuffer(transmit_buffer, sizeof(transmit_buffer)), \
    rxCallback(this, &Serial::rxCallback_), txCallback(this, &Serial::txCallback_)
{
}

void Serial::enable(void)
{
    uart.setRxCallback(&rxCallback);
    uart.setTxCallback(&txCallback);

    uart.enableInterrupt();
}

void Serial::printf(uint8_t* data, uint32_t size)
{
    uint8_t byte;

    uart.lock();

    txBuffer.write(data, size);

    txBuffer.read(&byte);
    uart.writeByte(byte);
}

void Serial::scanf(uint8_t* buffer, uint32_t size)
{
}

/*=============================== protected =================================*/

/*================================ private ==================================*/

void Serial::rxCallback_(void)
{
}

void Serial::txCallback_(void)
{
    uint8_t byte;

    if (txBuffer.read(&byte) == 0)
    {
        uart.writeByte(byte);
    }
    else
    {
        uart.unlockFromInterrupt();
    }
}
