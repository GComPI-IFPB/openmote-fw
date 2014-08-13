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

static const uint8_t RX_BUFFER_SIZE = 128;
static const uint8_t TX_BUFFER_SIZE = 128;

/*================================ typedef ==================================*/

/*=============================== variables =================================*/

/* Receive buffer */
uint8_t rx_buffer[RX_BUFFER_SIZE];
uint8_t* rxBuffer_head;
uint8_t* rxBuffer_tail;
uint32_t rxBuffer_size;

/* Transmit buffer */
uint8_t tx_buffer[TX_BUFFER_SIZE];
uint8_t* txBuffer_head;
uint8_t* txBuffer_tail;
uint32_t txBuffer_size;

/*=============================== prototypes ================================*/

/*================================= public ==================================*/

Serial::Serial(UartDriver& uart_):
    uart(uart_), rxCallback(this, &Serial::rxCallback_), txCallback(this, &Serial::txCallback_)
{
    rxBuffer_head = rx_buffer;
    rxBuffer_tail = rx_buffer;
    rxBuffer_size = 0;
    
    txBuffer_head = tx_buffer;
    txBuffer_tail = tx_buffer + sizeof(tx_buffer);
    txBuffer_size = 0;
}

void Serial::enable(void)
{
    uart.setRxCallback(&rxCallback);
    uart.setTxCallback(&txCallback);
    
    uart.interruptEnable();
}

void Serial::printf(uint8_t* data, uint32_t size)
{
    uint8_t* ptr;
    
    uart.lock();
    
    ptr = txBuffer_head;
    
    txBuffer_size += size;
    while (size-- && ptr != txBuffer_tail)
    {
        *ptr++ = *data++;
    }
    
    uart.writeByte(*txBuffer_head++);
    txBuffer_size--;
}

void Serial::scanf(uint8_t* buffer, uint32_t size)
{
}

/*================================ private ==================================*/

void Serial::rxCallback_(void)
{

}

void Serial::txCallback_(void)
{

    if (txBuffer_size > 0)
    {
        uart.writeByte(*txBuffer_head++);
        txBuffer_size--;
    }
    else
    {
        uart.unlock();
    }

}
