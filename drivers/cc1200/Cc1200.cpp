/**
 * @file       Cc1200.cpp
 * @author     Pere Tuset-Peiro (peretuset@openmote.com)
 * @version    v0.1
 * @date       May, 2016
 * @brief
 *
 * @copyright  Copyright 2016, OpenMote Technologies, S.L.
 *             This file is licensed under the GNU General Public License v2.
 */

/*================================ include ==================================*/

#include <stdint.h>

#include "Cc1200.h"
#include "Cc1200_regs.h"

#include "Gpio.h"
#include "Spi.h"

#include "platform_types.h"

/*================================ define ===================================*/

/*================================ typedef ==================================*/

/*=============================== variables =================================*/

/*=============================== prototypes ================================*/

/*================================= public ==================================*/

Cc1200::Cc1200(Spi& spi, GpioIn& gpio0, GpioIn& gpio2, GpioIn& gpio3):
    spi_(spi), gpio0_(gpio0), gpio2_(gpio2), gpio3_(gpio3)
{
}

void Cc1200::enable(void)
{
	strobe(CC1200_SXOFF);
}

void Cc1200::sleep(void)
{
}

void Cc1200::wakeup(void)
{
}

void Cc1200::on(void)
{
}

void Cc1200::off(void)
{
}

void Cc1200::reset(void)
{
}

void Cc1200::setRxCallbacks(Callback* rxInit, Callback* rxDone)
{
    /* Store the receive init and done callbacks */
    rxInit_ = rxInit;
    rxDone_ = rxDone;
}

void Cc1200::setTxCallbacks(Callback* txInit, Callback* txDone)
{
    /* Store the transmit init and done callbacks */
    txInit_ = txInit;
    txDone_ = txDone;
}

void Cc1200::enableInterrupts(void)
{
}

void Cc1200::disableInterrupts(void)
{
}

void Cc1200::setChannel(uint8_t channel)
{
}

void Cc1200::setPower(uint8_t power)
{
}

void Cc1200::transmit(void)
{
}

void Cc1200::receive(void)
{
}

RadioResult Cc1200::loadPacket(uint8_t* data, uint8_t length)
{
	return RadioResult_Error;
}

RadioResult Cc1200::getPacket(uint8_t* buffer, uint8_t* length, int8_t* rssi, uint8_t* lqi, uint8_t* crc)
{
	return RadioResult_Error;
}

/*=============================== protected =================================*/

/*================================ private ==================================*/

void Cc1200::strobe(uint8_t strobe)
{
	spi_.select();

    spi_.writeByte(strobe);

    spi_.deselect();
}

uint8_t Cc1200::state(void)
{
	return 0;
}