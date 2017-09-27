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

#include "Board.h"

#include "At86rf215.h"
#include "At86rf215_regs.h"

#include "platform_types.h"

/*================================ define ===================================*/

/*================================ typedef ==================================*/

/*=============================== variables =================================*/

extern Board board;

/*=============================== prototypes ================================*/

/*================================= public ==================================*/

At86rf215::At86rf215(Spi& spi, GpioOut& pwr, GpioOut& rst, GpioOut& cs, GpioIn& irq):
    spi_(spi), pwr_(pwr), rst_(rst), cs_(cs), irq_(irq),
    callback_(this, &At86rf215::interruptHandler)

{
    irq_.setCallback(&callback_);
}

void At86rf215::enable(void)
{
    pwr_.on();
    board.delayMilliseconds(1);
    rst_.low();
}

void At86rf215::sleep(void)
{
    rst_.high();
    board.delayMilliseconds(1);
    pwr_.off();
}

void At86rf215::wakeup(void)
{
}

void At86rf215::reset(void)
{
    rst_.high();
    board.delayMilliseconds(1);
    rst_.low();
}

bool At86rf215::check(void)
{
    uint8_t pn;
    uint8_t ver;

    singleAccessRead(AT86RF215_RF_PN_ADDR, &pn);

    if (pn == AT86RF215_RF_PN_215) {
        
        singleAccessRead(AT86RF215_RF_VN_ADDR, &ver);

        if (ver == AT86RF215_RF_VN_1_1 ||
            ver == AT86RF215_RF_VN_1_3)
        {
            return true;
        }
        else
        {
            return false;
        }

    }
    else
    {
        return false;
    }
}

void At86rf215::setCallbacks(Callback* rf09, Callback* rf24)
{
    /* Store the receive init and done callbacks */
    rf09_cb = rf09;
    rf24_cb = rf24;
}

void At86rf215::clearCallbacks(void)
{
    /* Re-store the receive init and done callbacks */
    rf09_cb = nullptr;
    rf24_cb = nullptr;
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

void At86rf215::interruptHandler(void)
{
    /* Read interrupt registers */
    uint8_t rf09, rf24;

    singleAccessRead(AT86RF215_RF_RFN09_IRQS_ADDR, &rf09);
    singleAccessRead(AT86RF215_RF_RFN24_IRQS_ADDR, &rf24);

    if (rf09 & AT86RF215_RF_RFn_IRQS_WAKEUP)
    {
        if (rf09_cb != nullptr)
        {
            rf09_cb->execute();
        }
    }

    if (rf24 & AT86RF215_RF_RFn_IRQS_WAKEUP)
    {
        if (rf24_cb != nullptr)
        {
            rf24_cb->execute();
        }
    }
}

/*================================ private ==================================*/

void At86rf215::singleAccessRead(uint16_t address, uint8_t* value)
{
    uint8_t spi_transaction[3];
    uint8_t address_hi, address_lo;

    address_hi = (uint8_t) ((address & 0xFF00) >> 8);
    address_lo = (uint8_t) ((address & 0x00FF) >> 0);

    spi_transaction[0] = AT86RF215_READ_CMD | address_hi;
    spi_transaction[1] = address_lo;
    spi_transaction[2] = 0x00;

    // Activate CS
    cs_.low();

    // Write the SPI transaction
    spi_.rwByte(spi_transaction, 3, spi_transaction, 3);

    // Deactivate CS
    cs_.high();

    *value = spi_transaction[2];
}

void At86rf215::singleAccessWrite(uint16_t address, uint8_t value)
{
    uint8_t spi_transaction[3];
    uint8_t address_hi, address_lo;

    address_hi = (uint8_t) ((address & 0xFF00) >> 8);
    address_lo = (uint8_t) ((address & 0x00FF) >> 0);

    spi_transaction[0] = AT86RF215_WRITE_CMD | address_hi;
    spi_transaction[1] = address_lo;
    spi_transaction[2] = value;

    // Activate CS
    cs_.low();

    // Write the SPI transaction
    spi_.rwByte(spi_transaction, 3, spi_transaction, 3);

    // Deactivate CS
    cs_.high();
}

void At86rf215::blockAccessRead(uint16_t address, uint8_t* values, uint16_t length)
{
    uint8_t address_hi, address_lo;

    address_hi = (uint8_t) ((address & 0xFF00) >> 8);
    address_lo = (uint8_t) ((address & 0x00FF) >> 0);

    values[0] = AT86RF215_READ_CMD | address_hi;
    values[1] = address_lo;

    // Activate CS
    cs_.low();

    for (uint16_t i = 0; i < length; i++)
    {
        // Read byte to the SPI interface
        uint8_t b = spi_.rwByte(values[i]);
        values[i] = b;
    }

    // Deactivate CS
    cs_.high();
}

void At86rf215::blockAccessWrite(uint16_t address, uint8_t* values, uint16_t length)
{
    uint8_t address_hi, address_lo;

    address_hi = (uint8_t) ((address & 0xFF00) >> 8);
    address_lo = (uint8_t) ((address & 0x00FF) >> 0);

    values[0] = AT86RF215_WRITE_CMD | address_hi;
    values[1] = address_lo;

    // Activate CS
    cs_.low();

    for (uint16_t i = 0; i < length; i++)
    {
        // Write byte to the SPI interface
        spi_.rwByte(values[i]);
    }

    // Deactivate CS
    cs_.high();
}
