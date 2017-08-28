/**
 * @file       At86rf215.cpp
 * @author     Pere Tuset-Peiro (peretuset@openmote.com)
 * @version    v0.1
 * @date       May, 2016
 * @brief
 *
 * @copyright  Copyright 2016, OpenMote Technologies, S.L.
 *             This file is licensed under the GNU General Public License v2.
 */

/*================================ include ==================================*/

#include "At86rf215.h"

#include "platform_types.h"

/*================================ define ===================================*/

/*================================ typedef ==================================*/

#define AT86RF215_READ 				( 0x00 )
#define AT86RF215_WRITE				( 0x80 )

#define AT86RF215_RF_PN_ADDR 		( 0x0D )
#define AT86RF215_RF_PN_215 		( 0x34 )
#define AT86RF215_RF_PN_215_IQ		( 0x35 )
#define AT86RF215_RF_PN_215_M		( 0x36 )

#define AT86RF215_RF_VER_ADDR		( 0x0E )
#define AT86RF215_RF_VER_1_1		( 0x01 )
#define AT86RF215_RF_VER_1_3		( 0x03 )

/*=============================== variables =================================*/

/*=============================== prototypes ================================*/

/*================================= public ==================================*/
At86rf215::At86rf215(Spi& spi, GpioOut& pwr, GpioOut& rst, GpioOut& cs, GpioIn& irq):
    spi_(spi), pwr_(pwr), rst_(rst), cs_(cs), irq_(irq)
{
	pwr_.high();
	rst_.high();
    cs_.high();
}

void At86rf215::enable(void)
{

}

void At86rf215::sleep(void)
{
	
}

void At86rf215::wakeup(void)
{
	
}

void At86rf215::on(void)
{
	pwr_.high();
	rst_.high();
}

void At86rf215::off(void)
{
	rst_.low();
	pwr_.low();
}

void At86rf215::reset(void)
{
	rst_.low();
	rst_.high();
}

bool At86rf215::check(void)
{
	uint8_t spi_transaction[3];
	uint8_t version;

	spi_transaction[2] = AT86RF215_READ;
	spi_transaction[1] = AT86RF215_RF_PN_ADDR;
	spi_transaction[0] = 0x00;

	// Activate CS
    cs_.low();

    // Write the SPI transaction
    spi_.rwByte(spi_transaction, 3, spi_transaction, 3);

    // Deactivate CS
    cs_.high();

    version = spi_transaction[0];

    if (version == AT86RF215_RF_PN_215) {
    	return true;
    } else {
    	return false;
    }
}

void At86rf215::setRxCallbacks(Callback* rxInit, Callback* rxDone)
{
	/* Store the receive init and done callbacks */
    rxInit_ = rxInit;
    rxDone_ = rxDone;
}

void At86rf215::setTxCallbacks(Callback* txInit, Callback* txDone)
{
	/* Store the transmit init and done callbacks */
    txInit_ = txInit;
    txDone_ = txDone;
}

void At86rf215::enableInterrupts(void)
{
	irq_.enableInterrupts();
}

void At86rf215::disableInterrupts(void)
{
	irq_.disableInterrupts();
}

/*=============================== protected =================================*/

/*================================ private ==================================*/
