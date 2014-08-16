/*
 * Copyright 2013 OpenMote Technologies, S.L.
 */

/**
 *
 * @file       Radio.cpp
 * @author     Pere Tuset-Peiro (peretuset@openmote.com)
 * @version    v0.1
 * @date       May, 2014
 * @brief
 * @ingroup
 *
 */

/*================================ include ==================================*/

#include "Radio.h"

#include "interrupt.h"
#include "ioc.h"
#include "sys_ctrl.h"

#include "hw_ints.h"
#include "hw_rfcore_sfr.h"
#include "hw_rfcore_xreg.h"
#include "hw_memmap.h"
#include "hw_types.h"

/*================================ define ===================================*/

// Defines for the transmit power
#define CC2538_RF_TX_POWER_RECOMMENDED          0xD5
#define CC2538_RF_TX_POWER_DEFAULT              0xD5

// Defines for the channel
#define CC2538_RF_CHANNEL_MIN                   11
#define CC2538_RF_CHANNEL_MAX                   26
#define CC2538_RF_CHANNEL_DEFAULT               17
#define CC2538_RF_CHANNEL_SPACING               5

// Defines for the packet
#define CC2538_RF_MAX_PACKET_LEN                127
#define CC2538_RF_MIN_PACKET_LEN                4

// Defines for the CCA (Clear Channel Assessment)
#define CC2538_RF_CCA_CLEAR                     0x01
#define CC2538_RF_CCA_BUSY                      0x00
#define CC2538_RF_CCA_THRESHOLD                 0xF8

// Defines for the CSP (Command Strobe Processor)
#define CC2538_RF_CSP_OP_ISRXON                 0xE3
#define CC2538_RF_CSP_OP_ISTXON                 0xE9
#define CC2538_RF_CSP_OP_ISTXONCCA              0xEA
#define CC2538_RF_CSP_OP_ISRFOFF                0xEF
#define CC2538_RF_CSP_OP_ISFLUSHRX              0xED
#define CC2538_RF_CSP_OP_ISFLUSHTX              0xEE

// Send an RX ON command strobe to the CSP
#define CC2538_RF_CSP_ISRXON()    \
  do { HWREG(RFCORE_SFR_RFST) = CC2538_RF_CSP_OP_ISRXON; } while(0)

// Send a TX ON command strobe to the CSP
#define CC2538_RF_CSP_ISTXON()    \
  do { HWREG(RFCORE_SFR_RFST) = CC2538_RF_CSP_OP_ISTXON; } while(0)

// Send a RF OFF command strobe to the CSP
#define CC2538_RF_CSP_ISRFOFF()   \
  do { HWREG(RFCORE_SFR_RFST) = CC2538_RF_CSP_OP_ISRFOFF; } while(0)

// Flush the RX FIFO
#define CC2538_RF_CSP_ISFLUSHRX()  do { \
  HWREG(RFCORE_SFR_RFST) = CC2538_RF_CSP_OP_ISFLUSHRX; \
  HWREG(RFCORE_SFR_RFST) = CC2538_RF_CSP_OP_ISFLUSHRX; \
} while(0)

// Flush the TX FIFO
#define CC2538_RF_CSP_ISFLUSHTX()  do { \
  HWREG(RFCORE_SFR_RFST) = CC2538_RF_CSP_OP_ISFLUSHTX; \
  HWREG(RFCORE_SFR_RFST) = CC2538_RF_CSP_OP_ISFLUSHTX; \
} while(0)

/*================================ typedef ==================================*/

/*=============================== variables =================================*/

/*=============================== prototypes ================================*/

/*================================= public ==================================*/

Radio::Radio()
{
}

void Radio::enable(void)
{
    CC2538_RF_CSP_ISRXON();
}

void Radio::sleep(void)
{
    // Wait for ongoing TX to complete (e.g. this could be an outgoing ACK)
    while(HWREG(RFCORE_XREG_FSMSTAT1) & RFCORE_XREG_FSMSTAT1_TX_ACTIVE);

    // Don't turn off if we are off as this will trigger a Strobe Error
    if(HWREG(RFCORE_XREG_RXENABLE) != 0) {
        //
        CC2538_RF_CSP_ISRFOFF();

        // Clear fifo isr flag
        HWREG(RFCORE_SFR_RFIRQF0) = ~(RFCORE_SFR_RFIRQF0_FIFOP | RFCORE_SFR_RFIRQF0_RXPKTDONE);
    }
}

void Radio::wakeup(void)
{
}

void Radio::transmit(void)
{
}

void Radio::receive(void)
{
}

void Radio::loadPacket(uint8_t* data, uint32_t length)
{
}

void Radio::getPacket(uint8_t* buffer, uint32_t length)
{
}

void Radio::registerRxInterrupts(Callback* rxInit_, Callback* rxDone_)
{
    if (rxInit_ != nullptr)
    {
        rxInit = rxInit_;
    }

    if (rxDone_ != nullptr)
    {
        rxDone = rxDone_;
    }
}

void Radio::registerTxInterrupts(Callback* txInit_, Callback* txDone_)
{
    if (txInit_ != nullptr)
    {
        txInit = txInit_;
    }

    if (txDone_ != nullptr)
    {
        txDone = txDone_;
    }
}

void Radio::enableInterrupts(void)
{
    IntEnable(INT_RFCORERTX);

    HWREG(RFCORE_XREG_RFERRM) = RFCORE_XREG_RFERRM_RFERRM_M;
    IntEnable(INT_RFCOREERR);

    /* Enable RF interrupts 0, RXPKTDONE,SFD,FIFOP only -- see page 751  */
    HWREG(RFCORE_XREG_RFIRQM0) |= ((0x06|0x02|0x01) << RFCORE_XREG_RFIRQM0_RFIRQM_S) & RFCORE_XREG_RFIRQM0_RFIRQM_M;

    /* Enable RF interrupts 1, TXDONE only */
    HWREG(RFCORE_XREG_RFIRQM1) |= ((0x02) << RFCORE_XREG_RFIRQM1_RFIRQM_S) & RFCORE_XREG_RFIRQM1_RFIRQM_M;
}

void Radio::disableInterrupts(void)
{
   /* Enable RF interrupts 0, RXPKTDONE,SFD,FIFOP only -- see page 751  */
   HWREG(RFCORE_XREG_RFIRQM0) = 0;

   /* Enable RF interrupts 1, TXDONE only */
   HWREG(RFCORE_XREG_RFIRQM1) = 0;
}

void Radio::setChannel(uint8_t channel)
{

    // Check that the channel is within bounds
    if((channel < CC2538_RF_CHANNEL_MIN) || (channel > CC2538_RF_CHANNEL_MAX))
    {
        while(true);
    }

    /* Changes to FREQCTRL take effect after the next recalibration */
    HWREG(RFCORE_XREG_FREQCTRL) = (CC2538_RF_CHANNEL_MIN +
                                  (channel - CC2538_RF_CHANNEL_MIN) * CC2538_RF_CHANNEL_SPACING);
}

void Radio::setPower(uint8_t power)
{
}

/*=============================== protected =================================*/

void Radio::interruptHandler(void)
{
    uint8_t irq_status0, irq_status1;

    // Read RFCORE_STATUS
    irq_status0 = HWREG(RFCORE_SFR_RFIRQF0);
    irq_status1 = HWREG(RFCORE_SFR_RFIRQF1);

    // Clear pending interrupt
    IntPendClear(INT_RFCORERTX);

    // Clear RFCORE_STATUS
    HWREG(RFCORE_SFR_RFIRQF0) = 0;
    HWREG(RFCORE_SFR_RFIRQF1) = 0;

    // STATUS0 Register
    // Start of frame event
    if ((irq_status0 & RFCORE_SFR_RFIRQF0_SFD) == RFCORE_SFR_RFIRQF0_SFD)
    {
        // txInit
        // rxInit
    }

    // STATUS0 Register
    // End of frame event
    if (((irq_status0 & RFCORE_SFR_RFIRQF0_RXPKTDONE) ==  RFCORE_SFR_RFIRQF0_RXPKTDONE))
    {
        // rxDone
    }

    // STATUS0 Register
    // Fifo is full event
    if (((irq_status0 & RFCORE_SFR_RFIRQF0_FIFOP) ==  RFCORE_SFR_RFIRQF0_FIFOP))
    {
        //
    }

    // STATUS1 Register
    // End of frame event
    if (((irq_status1 & RFCORE_SFR_RFIRQF1_TXDONE) == RFCORE_SFR_RFIRQF1_TXDONE))
    {
        // txDone
    }
}

void Radio::errorHandler(void)
{
    uint8_t rferrm;

    // Read RFERR_STATUS
    rferrm = (uint8_t) HWREG(RFCORE_XREG_RFERRM);

    // Clear pending interrupt
    IntPendClear(INT_RFCOREERR);

    if ((HWREG(RFCORE_XREG_RFERRM) & (((0x02) << RFCORE_XREG_RFERRM_RFERRM_S) & RFCORE_XREG_RFERRM_RFERRM_M)) & ((uint32_t)rferrm))
    {
        //
        HWREG(RFCORE_XREG_RFERRM) = ~(((0x02)<<RFCORE_XREG_RFERRM_RFERRM_S)&RFCORE_XREG_RFERRM_RFERRM_M);

        // ToDO: Handler error
    }
    else
    {
        while(true);
    }
}

/*================================ private ==================================*/
