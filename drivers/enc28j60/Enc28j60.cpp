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

#define EIE                 ( 0x1B )
#define EIR                 ( 0x1C )
#define ESTAT               ( 0x1D )
#define ECON2               ( 0x1E )
#define ECON1               ( 0x1F )

#define ESTAT_CLKRDY        ( 0x01 )
#define ESTAT_TXABRT        ( 0x02 )

#define ECON1_RXEN          ( 0x04 )
#define ECON1_TXRTS         ( 0x08 )

#define ECON2_AUTOINC       ( 0x80 )
#define ECON2_PKTDEC        ( 0x40 )

#define EIR_TXIF            ( 0x08 )

#define ERXTX_BANK          ( 0x00 )

#define ERDPTL              ( 0x00 )
#define ERDPTH              ( 0x01 )
#define EWRPTL              ( 0x02 )
#define EWRPTH              ( 0x03 )
#define ETXSTL              ( 0x04 )
#define ETXSTH              ( 0x05 )
#define ETXNDL              ( 0x06 )
#define ETXNDH              ( 0x07 )
#define ERXSTL              ( 0x08 )
#define ERXSTH              ( 0x09 )
#define ERXNDL              ( 0x0A )
#define ERXNDH              ( 0x0B )
#define ERXRDPTL            ( 0x0C )
#define ERXRDPTH            ( 0x0D )

#define RX_BUF_START        ( 0x0000 )
#define RX_BUF_END          ( 0x0FFF )

#define TX_BUF_START        ( 0x1200 )

/* MACONx registers are in bank 2 */
#define MACONX_BANK         ( 0x02 )
#define MACON1              ( 0x00 )
#define MACON2              ( 0x01 )
#define MACON3              ( 0x02 )
#define MACON4              ( 0x03 )
#define MABBIPG             ( 0x04 )
#define MAIPGL              ( 0x06 )
#define MAIPGH              ( 0x07 )
#define MAMXFLL             ( 0x0a )
#define MAMXFLH             ( 0x0b )

#define MACON1_TXPAUS       ( 0x08 )
#define MACON1_RXPAUS       ( 0x04 )
#define MACON1_MARXEN       ( 0x01 )

#define MACON2_MARST        ( 0x80 )

#define MACON3_PADCFG_FULL  ( 0xe0 )
#define MACON3_TXCRCEN      ( 0x10 )
#define MACON3_FRMLNEN      ( 0x02 )
#define MACON3_FULDPX       ( 0x01 )

#define MAX_MAC_LENGTH      ( 1518 )

#define MAADRX_BANK         ( 0x03 )
#define MAADR1              ( 0x04 ) /* MAADR<47:40> */
#define MAADR2              ( 0x05 ) /* MAADR<39:32> */
#define MAADR3              ( 0x02 ) /* MAADR<31:24> */
#define MAADR4              ( 0x03 ) /* MAADR<23:16> */
#define MAADR5              ( 0x00 ) /* MAADR<15:8> */
#define MAADR6              ( 0x01 ) /* MAADR<7:0> */

#define EPKTCNT_BANK        ( 0x01 )
#define ERXFCON             ( 0x18 )
#define EPKTCNT             ( 0x19 )

#define ERXFCON_UCEN        ( 0x80 )
#define ERXFCON_ANDOR       ( 0x40 )
#define ERXFCON_CRCEN       ( 0x20 )
#define ERXFCON_MCEN        ( 0x02 )
#define ERXFCON_BCEN        ( 0x01 )

/*================================ typedef ==================================*/

/*=============================== variables =================================*/

/*=============================== prototypes ================================*/

/*================================= public ==================================*/

Enc28j60::Enc28j60(SpiDriver& spi_, GpioIn& gpio_):
    spi(spi_), gpio(gpio_)
{
}

void Enc28j60::init(uint8_t* address)
{
    for(uint8_t i = 0; i < 6; i++)
    {
        mac_address[i] = *address++;
    }
}

void Enc28j60::reset(void)
{
    softReset();
}

int32_t Enc28j60::send(uint8_t* data, uint32_t length)
{
    return 0;
}

int32_t Enc28j60::read(uint8_t* buffer, uint32_t length)
{
    return 0;
}

/*=============================== protected =================================*/

/*================================ private ==================================*/

void Enc28j60::setRegBank(uint8_t bank)
{

}

uint8_t Enc28j60::readRegister(uint8_t address)
{
    return 0;
}

void Enc28j60::writeRegister(uint8_t address, uint8_t data)
{

}

uint8_t Enc28j60::readDataByte(void)
{
    return 0;
}

void Enc28j60::writeDataByte(uint8_t byte)
{

}

int32_t Enc28j60::readData(uint8_t* buffer, uint32_t length)
{
    return 0;
}

void Enc28j60::writeData(uint8_t* data, uint32_t length)
{

}

void Enc28j60::softReset(void)
{

}
