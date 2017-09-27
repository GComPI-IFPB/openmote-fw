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

#define AT86RF215_READ_CMD          ( 0x00 )
#define AT86RF215_WRITE_CMD         ( 0x80 )

#define AT86RF215_RF_PN_ADDR        ( 0x000D )
#define AT86RF215_RF_PN_215         ( 0x34 )
#define AT86RF215_RF_PN_215_IQ      ( 0x35 )
#define AT86RF215_RF_PN_215_M       ( 0x36 )

#define AT86RF215_RF_VN_ADDR        ( 0x000E )
#define AT86RF215_RF_VN_1_1         ( 0x01 )
#define AT86RF215_RF_VN_1_3         ( 0x03 )

#define AT86RF215_RF_CFG_ADDR       ( 0x0006 )
#define AT86RF215_RF_CFG_IRQMM_0    ( 0 << 3 )
#define AT86RF215_RF_CFG_IRQMM_1    ( 1 << 3 )
#define AT86RF215_RF_CFG_IRQP_0     ( 0 << 2 )
#define AT86RF215_RF_CFG_IRQP_1     ( 1 << 2 )
#define AT86RF215_RF_CFG_IDRV_2     ( 0 << 0 )
#define AT86RF215_RF_CFG_IDRV_4     ( 1 << 0 )
#define AT86RF215_RF_CFG_IDRV_6     ( 2 << 0 )
#define AT86RF215_RF_CFG_IDRV_8     ( 3 << 0 )

#define AT86RF215_RF_CLKO_ADDR      ( 0x0007 )
#define AT86RF215_RF_CLKO_DRV_0     ( 0 << 3 )
#define AT86RF215_RF_CLKO_DRV_1     ( 1 << 3 )
#define AT86RF215_RF_CLKO_DRV_2     ( 2 << 3 )
#define AT86RF215_RF_CLKO_DRV_3     ( 3 << 3 )
#define AT86RF215_RF_CLKO_OS_OFF    ( 0 << 0 )
#define AT86RF215_RF_CLKO_OS_26M    ( 1 << 0 )
#define AT86RF215_RF_CLKO_OS_32M    ( 2 << 0 )
#define AT86RF215_RF_CLKO_OS_16M    ( 3 << 0 )
#define AT86RF215_RF_CLKO_OS_8M     ( 4 << 0 )
#define AT86RF215_RF_CLKO_OS_4M     ( 5 << 0 )
#define AT86RF215_RF_CLKO_OS_2M     ( 6 << 0 )
#define AT86RF215_RF_CLKO_OS_1M     ( 7 << 0 )

#define AT86RF215_RF_RST_ADDR       ( 0x0005 )
#define AT86RF215_RF_RST_CMD        ( 0x07 )

#define AT86RF215_RF_IQIFC1_ADDR    ( 0x0005 )
#define AT86RF215_RF_IQIFC1_BBRF    ( 0 << 4 )
#define AT86RF215_RF_IQIFC1_RF      ( 1 << 4 )
#define AT86RF215_RF_IQIFC1_BBRF09  ( 4 << 4 )
#define AT86RF215_RF_IQIFC1_BBRF24  ( 5 << 4 )

#define AT86RF215_RF_RN09_STATE_ADDR     (  )
#define AT86RF215_RF_RN24_STATE_ADDR     (  )

#define AT86RF215_RF_STATE_TRXOFF   ( 2 << 0 )
#define AT86RF215_RF_STATE_TXPREP   ( 3 << 0 )
#define AT86RF215_RF_STATE_TX       ( 4 << 0 )
#define AT86RF215_RF_STATE_RX       ( 5 << 0 )
#define AT86RF215_RF_STATE_TRANS    ( 6 << 0 )
#define AT86RF215_RF_STATE_RESET    ( 7 << 0 )

#define AT86RF215_RF_RN09_CMD_ADDR      (  )
#define AT86RF215_RF_RN24_CMD_ADDR      (  )

#define AT86RF215_RF_CMD_NOP            ( 0 << 0 )
#define AT86RF215_RF_CMD_SLEEP          ( 1 << 0 )
#define AT86RF215_RF_CMD_TRXOFF         ( 2 << 0 )
#define AT86RF215_RF_CMD_TXPREP         ( 3 << 0 )
#define AT86RF215_RF_CMD_TX             ( 4 << 0 )
#define AT86RF215_RF_CMD_RX             ( 5 << 0 )
#define AT86RF215_RF_CMD_RESET          ( 7 << 0 )

#define AT86RF215_RF_RFN09_IRQS_ADDR    ( 0x0000 )
#define AT86RF215_RF_RFN24_IRQS_ADDR    ( 0x0001 )
#define AT86RF215_RF_BBC0_IRQS_ADDR     ( 0x0002 )
#define AT86RF215_RF_BBC1_IRQS_ADDR     ( 0x0003 )
#define AT86RF215_RF_RFN09_IRQM_ADDR    ( 0x0100 )
#define AT86RF215_RF_RFN24_IRQM_ADDR    ( 0x0200 )
#define AT86RF215_RF_BBC0_IRQM_ADDR     ( 0x0300 )
#define AT86RF215_RF_BBC1_IRQM_ADDR     ( 0x0400 )

#define AT86RF215_RF_BBCn_IRQM_FBLI_(X)   ( X << 7 )
#define AT86RF215_RF_BBCn_IRQM_AGCR_(X)   ( X << 6 )
#define AT86RF215_RF_BBCn_IRQM_AGCH_(X)   ( X << 5 )
#define AT86RF215_RF_BBCn_IRQM_TXFE_(X)   ( X << 4 )
#define AT86RF215_RF_BBCn_IRQM_RXEM_(X)   ( X << 3 )
#define AT86RF215_RF_BBCn_IRQM_RXAM_(X)   ( X << 2 )
#define AT86RF215_RF_BBCn_IRQM_RXFE_(X)   ( X << 1 )
#define AT86RF215_RF_BBCn_IRQM_RXFS_(X)   ( X << 0 )

#define AT86RF215_RF_RFn_IRQM_IQIFSF_(X)    ( X << 5 )
#define AT86RF215_RF_RFn_IRQM_TRXERR_(X)    ( X << 4 )
#define AT86RF215_RF_RFn_IRQM_BATLOW_(X)    ( X << 3 )
#define AT86RF215_RF_RFn_IRQM_EDC_(X)       ( X << 2 )
#define AT86RF215_RF_RFn_IRQM_TRXRDY_(X)    ( X << 1 )
#define AT86RF215_RF_RFn_IRQM_WAKEUP_(X)    ( X << 0 )

#define AT86RF215_RF_RFn_IRQS_IQIFSF    ( 0x20 )
#define AT86RF215_RF_RFn_IRQS_TRXERR    ( 0x10 )
#define AT86RF215_RF_RFn_IRQS_BATLOW    ( 0x08 )
#define AT86RF215_RF_RFn_IRQS_EDC       ( 0x04 )
#define AT86RF215_RF_RFn_IRQS_TRXRDY    ( 0x02 )
#define AT86RF215_RF_RFn_IRQS_WAKEUP    ( 0x01 )

#define AT86RF215_RF_BBCn_IRQS_FBLI   ( 0x80 )
#define AT86RF215_RF_BBCn_IRQS_AGCR   ( 0x40 )
#define AT86RF215_RF_BBCn_IRQS_AGCH   ( 0x20 )
#define AT86RF215_RF_BBCn_IRQS_TXFE   ( 0x10 )
#define AT86RF215_RF_BBCn_IRQS_RXEM   ( 0x08 )
#define AT86RF215_RF_BBCn_IRQS_RXAM   ( 0x04 )
#define AT86RF215_RF_BBCn_IRQS_RXFE   ( 0x02 )
#define AT86RF215_RF_BBCn_IRQS_RXFS   ( 0x01 )

/*=============================== variables =================================*/

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
}

void At86rf215::sleep(void)
{
    
}

void At86rf215::wakeup(void)
{
    
}

void At86rf215::on(void)
{
    pwr_.on();
    rst_.low(); 
}

void At86rf215::off(void)
{
    cs_.low();
    rst_.high();
}

void At86rf215::reset(void)
{
    rst_.high();
    // Add delay
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
