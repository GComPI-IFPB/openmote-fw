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

#include "Cc1200.h"

#include "platform_types.h"

/*================================ define ===================================*/

// Read out packet on falling edge of GPIO0
#define GPIO0_IOCFG                     CC1200_IOCFG_PKT_SYNC_RXTX

// Arbitrary configuration for GPIO3
#define GPIO3_IOCFG                     CC1200_IOCFG_MARC_2PIN_STATUS_0

// CC1200 radio states
#define STATE_IDLE                      CC1200_STATUS_BYTE_IDLE
#define STATE_RX                        CC1200_STATUS_BYTE_RX
#define STATE_TX                        CC1200_STATUS_BYTE_TX
#define STATE_FSTXON                    CC1200_STATUS_BYTE_FSTXON
#define STATE_CALIBRATE                 CC1200_STATUS_BYTE_CALIBRATE
#define STATE_SETTLING                  CC1200_STATUS_BYTE_SETTLING
#define STATE_RX_FIFO_ERR               CC1200_STATUS_BYTE_RX_FIFO_ERR
#define STATE_TX_FIFO_ERR               CC1200_STATUS_BYTE_TX_FIFO_ERR

/*================================ typedef ==================================*/

/*=============================== variables =================================*/

/*=============================== prototypes ================================*/

/*================================= public ==================================*/


Cc1200::Cc1200(Spi& spi, GpioOut& cs, GpioIn& gpio0, GpioIn& gpio2, GpioIn& gpio3):
    spi_(spi), cs_(cs), gpio0_(gpio0), gpio2_(gpio2), gpio3_(gpio3)
{
    cs_.high();
}

void Cc1200::init(void)
{
    reset();
}

void Cc1200::configure(cc1200_config_pair_t* config, uint32_t size)
{
    uint32_t elements = size / sizeof(cc1200_config_pair_t);

    for (uint32_t i = 0; i < elements; ++i) {
        singleWrite(config[i].address, config[i].value);
    }
}

void Cc1200::sleep(void)
{
    // Make sure we are in IDLE mode before going to sleep
    idle();

    // Reconfigure IOCFG0 for detecting wake-up condition
    singleWrite(CC1200_IOCFG0, CC1200_IOCFG_RXFIFO_CHIP_RDY_N);

    // Go to lowest power mode, some registers are retained
    strobe(CC1200_SPWD);
}

void Cc1200::wakeup(void)
{
    on();
}

void Cc1200::on(void)
{
    bool s;
    
    // Pull CS low to wake-up the chip
    cs_.low();
    
    // Wait until CHIP_RDYn is low
    s = gpio0_.read();
    while (s == true) {
        s = gpio0_.read();
    }

    // Pull CS high to release the SPI
    cs_.high();

    // Write appropriate GPIO config
    singleWrite(CC1200_IOCFG0, GPIO0_IOCFG);

    // Calibrate the radio when turning it on
    // calibrate();
}

void Cc1200::off(void)
{
    // Make sure we are in IDLE mode before going to off
    idle();

    // Reconfigure IOCFG0 for detecting wake-up condition
    singleWrite(CC1200_IOCFG0, CC1200_IOCFG_RXFIFO_CHIP_RDY_N);

    // Power-off crystal to save power, all registers are retained
    strobe(CC1200_SXOFF);
}

void Cc1200::idle(void)
{
    uint8_t s;

    // Read the current state
    s = state();

    // Take action based on state:
    // If already idle, do nothing
    // If RX FIFO error, empty RX FIFO
    // If TX FIFO error, empty TX FIFO
    if (s == STATE_IDLE) {
        return;
    } else if (s == STATE_RX_FIFO_ERR) {
        strobe(CC1200_SFRX);
    } else if (s == STATE_TX_FIFO_ERR) {
        strobe(CC1200_SFTX);
    }

    // Now, put the radio in idle mode
    strobe(CC1200_SIDLE);

    // Wait until radio is in idle mode
    s = state();
    while(s != STATE_IDLE) {
        s = state();
    }
}

void Cc1200::reset(void)
{
    uint8_t s;

    // Reconfigure IOCFG0 for detecting wake-up condition
    singleWrite(CC1200_IOCFG0, CC1200_IOCFG_RXFIFO_CHIP_RDY_N);

    // Activate CS
    cs_.low();

    // Write the reset command strobe
    spi_.writeByte(CC1200_SRES);
    
    // Wait until CHIP_RDYn is low
    s = gpio0_.read();
    while (s == true) {
        s = gpio0_.read();
    }

    // Deactivate CS
    cs_.high();

    // Write appropriate GPIO config
    singleWrite(CC1200_IOCFG0, GPIO0_IOCFG);
}

void Cc1200::transmit(void)
{
    // Put the radio in transmit mode
    strobe(CC1200_STX);
}

void Cc1200::receive(void)
{
    // Put the radio in receive mode
    strobe(CC1200_SRX);
}

void Cc1200::calibrate(void)
{
    uint8_t s;

    // Put the radio in calibration mode
    strobe(CC1200_SCAL);

    // Wait until radio is in idle mode
    s = state();
    while(s != STATE_IDLE) {
        s = state();
    }
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

uint8_t Cc1200::strobe(uint8_t strobe)
{
    uint8_t result;

    // Activate CS
    cs_.low();

    // Write the strobe command
    result = spi_.rwByte(strobe);

    // Deactivate CS
    cs_.high();

    return result;
}

uint8_t Cc1200::state(void)
{
    uint8_t state;
    // Get the current radio state
    state = strobe(CC1200_SNOP) & 0x70;
    return state;
}

uint8_t Cc1200::singleRead(uint16_t address)
{
    uint8_t ret;

    // Activate CS
    cs_.low();

    // Check register addressing mode
    if (CC1200_IS_EXTENDED_ADDR(address)) {
        spi_.writeByte(CC1200_EXTENDED_READ_CMD);
        spi_.writeByte((uint8_t)address);
    } else {
        spi_.writeByte(address | CC1200_READ_BIT);
    }
    
    // Push a null byte to read a byte
    ret = spi_.readByte();
    
    // Deactivate CS
    cs_.high();

    return ret;
}

uint8_t Cc1200::singleWrite(uint16_t address, uint8_t value)
{
    uint8_t ret;

    // Activate CS
    cs_.low();

    // Check register addressing mode
    if (CC1200_IS_EXTENDED_ADDR(address)) {
        spi_.writeByte(CC1200_EXTENDED_WRITE_CMD);
        spi_.writeByte((uint8_t)address);
    } else {
        spi_.writeByte(address | CC1200_WRITE_BIT);
    }
    
    // Push the value and read the byte
    spi_.writeByte(value);
    
    // Deactivate CS
    cs_.high();

    return ret;
}