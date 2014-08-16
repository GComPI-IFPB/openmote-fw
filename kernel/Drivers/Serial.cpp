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
    uart(uart_), rxCallback(this, &Serial::rxCallback_), txCallback(this, &Serial::txCallback_)
{
    rxBufferHead = rxBuffer;
    rxBufferTail = rxBuffer + sizeof(rxBuffer);
    rxBufferSize = 0;

    txBufferHead = txBuffer;
    txBufferTail = txBuffer + sizeof(txBuffer);
    txBufferSize = 0;
}

void Serial::enable(void)
{
    uart.setRxCallback(&rxCallback);
    uart.setTxCallback(&txCallback);

    uart.interruptEnable();
}

void Serial::printf(uint8_t* data, uint32_t size)
{
    uart.lock();

    txBufferHead = txBuffer;

    while (size-- && (txBufferHead != txBufferTail))
    {
        *txBufferHead++ = *data++;
        txBufferSize++;
    }

    txBufferHead = txBuffer;

    uart.writeByte(*txBufferHead++);
    txBufferSize--;
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
    if (txBufferSize > 0)
    {
        uart.writeByte(*txBufferHead++);
        txBufferSize--;
    }
    else
    {
        uart.unlock();
    }
}
