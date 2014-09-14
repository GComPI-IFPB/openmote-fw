/*
 * Copyright 2013 OpenMote Technologies, S.L.
 */

/**
 *
 * @file       Enc28j60.cpp
 * @author     Pere Tuset-Peiro (peretuset@openmote.com)
 * @version    v0.1
 * @date       May, 2014
 * @brief
 * @ingroup
 *
 */

/*================================ include ==================================*/

#include "Enc28j60.h"

/*================================ define ===================================*/

#define EIE                     ( 0x1B )

#define EIR                     ( 0x1C )
#define EIR_TXIF                ( 0x08 )
#define EIR_                    (  )

#define ESTAT                   ( 0x1D )
#define ESTAT_TXABRT            ( 0x02 )
#define ESTAT_CLKRDY            ( 0x01 )
#define ECON1_                  (  )

#define ECON2                   ( 0x1E )
#define ECON2_AUTOINC           ( 0x80 )
#define ECON2_PKTDEC            ( 0x40 )
#define ECON2_                  (  )

#define ECON1                   ( 0x1F )
#define ECON1_TXRTS             ( 0x08 )
#define ECON1_RXEN              ( 0x04 )
#define ECON1_                  (  )

#define ERXTX_BANK              ( 0x00 )
#define ERDPTL                  ( 0x00 )
#define ERDPTH                  ( 0x01 )
#define EWRPTL                  ( 0x02 )
#define EWRPTH                  ( 0x03 )
#define ETXSTL                  ( 0x04 )
#define ETXSTH                  ( 0x05 )
#define ETXNDL                  ( 0x06 )
#define ETXNDH                  ( 0x07 )
#define ERXSTL                  ( 0x08 )
#define ERXSTH                  ( 0x09 )
#define ERXNDL                  ( 0x0A )
#define ERXNDH                  ( 0x0B )
#define ERXRDPTL                ( 0x0C )
#define ERXRDPTH                ( 0x0D )

#define MACONX_BANK             ( 0x02 )

#define MACON1                  ( 0x00 )
#define MACON1_TXPAUS           ( 0x08 )
#define MACON1_RXPAUS           ( 0x04 )
#define MACON1_MARXEN           ( 0x01 )
#define MACON1_                 (  )

#define MACON2                  ( 0x01 )
#define MACON2_MARST            ( 0x80 )
#define MACON2_                 (  )

#define MACON3                  ( 0x02 )
#define MACON3_PADCFG_FULL      ( 0xE0 )
#define MACON3_TXCRCEN          ( 0x10 )
#define MACON3_FRMLNEN          ( 0x02 )
#define MACON3_FULDPX           ( 0x01 )
#define MACON3_                 (  )

#define MACON4                  ( 0x03 )
#define MACON4_                 ( )

#define MABBIPG                 ( 0x04 )
#define MAIPGL                  ( 0x06 )
#define MAIPGH                  ( 0x07 )
#define MAMXFLL                 ( 0x0A )
#define MAMXFLH                 ( 0x0B )

#define MAADRX_BANK             ( 0x03 )
#define MAADR0                  ( 0x01 ) // MAADR<7:0>
#define MAADR1                  ( 0x00 ) // MAADR<15:8>
#define MAADR2                  ( 0x03 ) // MAADR<23:16>
#define MAADR3                  ( 0x02 ) // MAADR<31:24>
#define MAADR4                  ( 0x05 ) // MAADR<39:32>
#define MAADR5                  ( 0x04 ) // MAADR<47:40>

#define EPKTCNT_BANK            ( 0x01 )

#define ERXFCON                 ( 0x18 )
#define ERXFCON_UCEN            ( 0x80 )
#define ERXFCON_ANDOR           ( 0x40 )
#define ERXFCON_CRCEN           ( 0x20 )
#define ERXFCON_MCEN            ( 0x02 )
#define ERXFCON_BCEN            ( 0x01 )
#define ERXFCON_                (  )

#define EPKTCNT                 ( 0x19 )

/********* Configuration **********/

#define RX_BUFFER_START         ( 0x0000 )
#define RX_BUFFER_END           ( 0x0FFF )
#define TX_BUFFER_START         ( 0x1200 )
#define TX_BUFFER_END           ( 0x17EE )

#define MAC_FRAME_MAX_LENGTH    ( 1518 )

/*================================ typedef ==================================*/

/*=============================== variables =================================*/

uint8_t buffer[MAC_FRAME_MAX_LENGTH];
uint8_t* buffer_ptr = buffer;
uint32_t buffer_len = sizeof(buffer);

/*=============================== prototypes ================================*/

/*================================= public ==================================*/

Enc28j60::Enc28j60(SpiDriver& spi_, GpioIn& gpio_):
    EthernetDevice(), \
    spi(spi_), gpio(gpio_), \
    callback(this, &Enc28j60::interruptHandler)
{
}

void Enc28j60::init(uint8_t* mac_address)
{
    // Check if ENC28J60 is initialized
    if (isInitialized == true)
    {
        return;
    }

    // Set the MAC address
    setMacAddress(mac_address);

    // Initialize the ENC28J60 chip
    reset();

    // ENC28J60 chip is now initialized
    isInitialized = true;
}

void Enc28j60::reset(void)
{
    // Wait until clock is ready
    while ((readRegister(ESTAT) & ESTAT_CLKRDY) == 0)
        ;

    // Perform a software reset
    softReset();

    // Software delay
    for (register uint32_t i = 0x0FFF; i != 0; i--)
        ;

    // Set register bank
    setRegBank(ERXTX_BANK);

    // Configure receive buffer
    writeRegister(ERXSTL, RX_BUFFER_START & 0xFF);
    writeRegister(ERXSTH, RX_BUFFER_START >> 8);
    writeRegister(ERXNDL, RX_BUFFER_END & 0xFF);
    writeRegister(ERXNDH, RX_BUFFER_END >> 8);
    writeRegister(ERDPTL, RX_BUFFER_START & 0xFF);
    writeRegister(ERDPTH, RX_BUFFER_START >> 8);
    writeRegister(ERXRDPTL, RX_BUFFER_START & 0xFF);
    writeRegister(ERXRDPTH, RX_BUFFER_START >> 8);

    // Set register bank
    setRegBank(EPKTCNT_BANK);

    // Set receive filters
    writeRegister(ERXFCON, 0x00); // Done afterwards?

    // Set register bank
    setRegBank(MACONX_BANK);

    // Pull MAC out of reset
    writeRegister(MACON2, 0);

    // Turn on reception and IEEE-defined flow control
    writeRegister(MACON1, readRegister(MACON1) |
                 (MACON1_MARXEN | MACON1_TXPAUS | MACON1_RXPAUS));

    // Set Padding, CRC and Full-Duplex
    writeRegister(MACON3, readRegister(MACON3) |
                 (MACON3_PADCFG_FULL | MACON3_TXCRCEN | MACON3_FULDPX | MACON3_FRMLNEN));

    // Set maximum frame length
    writeRegister(MAMXFLL, MAC_FRAME_MAX_LENGTH & 0xFF);
    writeRegister(MAMXFLH, MAC_FRAME_MAX_LENGTH >> 8);

    // Set back-to-back inter packet gap
    writeRegister(MABBIPG, 0x15);

    // Set non-back-to-back packet gap
    writeRegister(MAIPGL, 0x12);
    writeRegister(MAIPGH, 0x0C);

    // Set register bank
    setRegBank(MAADRX_BANK);

    // Set MAC address
    writeRegister(MAADR0, macAddress[5]);
    writeRegister(MAADR1, macAddress[4]);
    writeRegister(MAADR2, macAddress[3]);
    writeRegister(MAADR3, macAddress[2]);
    writeRegister(MAADR4, macAddress[1]);
    writeRegister(MAADR5, macAddress[0]);

    // Set register bank
    setRegBank(EPKTCNT_BANK);

    // Set receive filters
    writeRegister(ERXFCON, ERXFCON_UCEN | ERXFCON_CRCEN | ERXFCON_BCEN);

    // Turn on autoincrement for buffer access
    writeRegister(ECON2, readRegister(ECON2) | ECON2_AUTOINC);

    // Turn on reception
    writeRegister(ECON1, ECON1_RXEN);
}

OperationResult Enc28j60::transmitFrame(uint8_t* data, uint32_t length)
{
    if (!isInitialized)
    {
        return ResultError;
    }

    // Set register bank
    setRegBank(ERXTX_BANK);

    // Set up the transmit buffer pointer
    writeRegister(ETXSTL, TX_BUFFER_START & 0xFF);
    writeRegister(ETXSTH, TX_BUFFER_START >> 8);
    writeRegister(EWRPTL, TX_BUFFER_START & 0xFF);
    writeRegister(EWRPTH, TX_BUFFER_START >> 8);

    // Do not override MACON3
    writeDataByte(0x00);

    // Write data to the buffer
    writeData(data, length);

    // Write the pointer to the last data byte
    writeRegister(ETXNDL, (TX_BUFFER_START + length) & 0xFF);
    writeRegister(ETXNDH, (TX_BUFFER_START + length) >> 8);

    // Clear transmit interrupt flag
    writeRegister(EIR, readRegister(EIR) & (~EIR_TXIF));

    // Enable packet transmission
    writeRegister(ECON1, readRegister(ECON1) | ECON1_TXRTS);

    // Busy-wait until packet is completed
    while((readRegister(ECON1) & ECON1_TXRTS) != 0)
        ;

    // Check packet transmission outcome
    if ((readRegister(ESTAT) & ESTAT_TXABRT) != 0)
    {
        return ResultError;
    }
    else
    {
        return ResultSuccess;
    }
}

OperationResult Enc28j60::receiveFrame(uint8_t* buffer, uint32_t* length)
{
    uint32_t status;
    uint32_t nextPacket;
    uint32_t packetLength;
    uint8_t packetCount;
    bool error = false;
    uint8_t temp[2];

    // Set register bank
    setRegBank(EPKTCNT_BANK);

    // Read number of packets
    packetCount = readRegister(EPKTCNT);

    // Return immediately if there are no packets
    if (packetCount == 0)
    {
        return ResultError;
    }

    // Set register bank
    setRegBank(ERXTX_BANK);

    // Read the next packet pointer
    temp[0] = readDataByte();
    temp[1] = readDataByte();
    nextPacket = ((temp[1] << 8) | temp[0]);

    // Read the packet length
    temp[0] = readDataByte();
    temp[1] = readDataByte();
    packetLength = ((temp[1] << 8) | temp[0]);

    // Read the current status
    temp[0] = readDataByte();
    temp[1] = readDataByte();
    status = ((temp[1] << 8) | temp[0]);
    status = status; // Avoid compiler warning

    // Check for buffer overflow
    if (*length >= packetLength)
    {
        // Update the packet length
        *length = packetLength;

        // Read received data to the buffer
        readData(buffer, packetLength);
    }
    else
    {
        // Buffer overflow
        error = true;

        // Read and discard bytes
        for (uint32_t i = 0; i < packetLength; i++)
        {
            readDataByte();
        }
    }

    // Read an additional byte at odd lengths to avoid FIFO corruption
    if ((packetLength % 2) != 0)
    {
        readDataByte();
    }

    /* Errata #14 */
    if (nextPacket == RX_BUFFER_START)
    {
        nextPacket = RX_BUFFER_END;
    }
    else
    {
        nextPacket -= 1;
    }
    writeRegister(ERXRDPTL, (nextPacket >> 0) & 0xFF);
    writeRegister(ERXRDPTH, (nextPacket >> 8) & 0xFF);

    // Reduce the number of received packets
    writeRegister(ECON2, readRegister(ECON2) | ECON2_PKTDEC);

    // If error, return immediately
    if (error == true) {
        return ResultError;
    }

    return ResultSuccess;
}

/*=============================== protected =================================*/

void Enc28j60::interruptHandler(void)
{
    buffer_ptr = buffer;
    buffer_len = sizeof(buffer);
    receiveFrame(buffer_ptr, &buffer_len);
}

/*================================ private ==================================*/

void Enc28j60::softReset(void)
{
    spi.select();
    spi.writeByte(0xFF);
    spi.deselect();
}

void Enc28j60::setRegBank(uint8_t bank)
{
    spi.select();
    writeRegister(ECON1, (readRegister(ECON1) & 0xFC) | (bank & 0x03));
    spi.deselect();
}

uint8_t Enc28j60::readRegister(uint8_t address)
{
    uint8_t value;

    spi.select();
    spi.writeByte(0x00 | (address & 0x1F));
    value = spi.readByte();
    spi.deselect();

    return value;
}

void Enc28j60::writeRegister(uint8_t address, uint8_t data)
{
    spi.select();
    spi.writeByte(0x40 | (address & 0x1F));
    spi.writeByte(data);
    spi.deselect();
}

uint8_t Enc28j60::readDataByte(void)
{
    uint8_t value;

    spi.select();
    spi.writeByte(0x3A);
    value = spi.readByte();
    spi.deselect();

    return value;
}

void Enc28j60::writeDataByte(uint8_t byte)
{
    spi.select();
    spi.writeByte(0x7A);
    spi.writeByte(byte);
    spi.deselect();
}

uint32_t Enc28j60::readData(uint8_t* buffer, uint32_t length)
{
    uint32_t i;

    spi.select();
    spi.writeByte(0x3A);
    for (i = 0; i < length; i++)
    {
        buffer[i] = spi.readByte();
    }
    spi.deselect();

    return i;
}

void Enc28j60::writeData(uint8_t* data, uint32_t length)
{
    uint32_t i;

    spi.select();
    spi.writeByte(0x7A);
    for (i = 0; i < length; i++)
    {
        spi.writeByte(data[i]);
    }
    spi.deselect();
}
