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
#include "hw_ana_regs.h"
#include "hw_rfcore_sfr.h"
#include "hw_rfcore_xreg.h"
#include "hw_memmap.h"
#include "hw_types.h"

/*================================ define ===================================*/

// Defines for the transmit power
#define CC2538_RF_TX_POWER_DEFAULT              0xD5

// Defines for the channel
#define CC2538_RF_CHANNEL_MIN                   11
#define CC2538_RF_CHANNEL_MAX                   26
#define CC2538_RF_CHANNEL_DEFAULT               17
#define CC2538_RF_CHANNEL_SPACING               5

#define CC2538_RF_RSSI_OFFSET                   73

#define CC2538_RF_CRC_BITMASK                   0x80
#define CC2538_RF_LQI_BITMASK                   0x7F

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
    /* Adjust for optimal radio performance */
    HWREG(RFCORE_XREG_MDMCTRL1)    = 0x14;
    HWREG(RFCORE_XREG_RXCTRL)      = 0x3F;
    
    /* Adjust current in synthesizer */
    HWREG(RFCORE_XREG_FSCTRL)      = 0x55;
   
    /* Tune sync word detection by requiring two zero symbols before the sync word */
    HWREG(RFCORE_XREG_MDMCTRL0)    = 0x85;
   
    /* Adjust current in VCO */
    HWREG(RFCORE_XREG_FSCAL1)      = 0x01;
   
    /* Adjust target value for AGC control loop */
    HWREG(RFCORE_XREG_AGCCTRL1)    = 0x15;
   
    /* Tune ADC performance */
    HWREG(RFCORE_XREG_ADCTEST0)    = 0x10;
    HWREG(RFCORE_XREG_ADCTEST1)    = 0x0E;
    HWREG(RFCORE_XREG_ADCTEST2)    = 0x03;
   
    /* Update CCA register to -81 dB */
    HWREG(RFCORE_XREG_CCACTRL0)    = 0xF8;

    /* Set transmit anti-aliasing filter bandwidth */
    HWREG(RFCORE_XREG_TXFILTCFG)   = 0x09;

    /* Set AGC target value */
    HWREG(RFCORE_XREG_AGCCTRL1)    = 0x15;

    /* Set bias currents */
    HWREG(ANA_REGS_O_IVCTRL)       = 0x0B;
   
    /* Disable the CSPT register compare function */
    HWREG(RFCORE_XREG_CSPT)        = 0xFFUL;
   
    /* Enable automatic CRC calculation and RSSI append */
    HWREG(RFCORE_XREG_FRMCTRL0)    = RFCORE_XREG_FRMCTRL0_AUTOCRC;
   
    /* Disable frame filtering */
    HWREG(RFCORE_XREG_FRMFILT0)   &= ~RFCORE_XREG_FRMFILT0_FRAME_FILTER_EN;
   
    /* Disable source address matching and autopend */
    HWREG(RFCORE_XREG_SRCMATCH)    = 0;
   
    /* Set maximum FIFOP threshold */
    HWREG(RFCORE_XREG_FIFOPCTRL)   = CC2538_RF_MAX_PACKET_LEN;
   
    /* Flush transmit and receive */
    CC2538_RF_CSP_ISFLUSHRX();
    CC2538_RF_CSP_ISFLUSHTX();
   
    /* Set default transmit power and channel */
    HWREG(RFCORE_XREG_TXPOWER)     = CC2538_RF_TX_POWER_DEFAULT;
    HWREG(RFCORE_XREG_FREQCTRL)    = CC2538_RF_CHANNEL_MIN;
}

void Radio::sleep(void)
{
    off();
}

void Radio::wakeup(void)
{
}

void Radio::reset(void)
{
    /* Wait for ongoing TX to complete (e.g. this could be an outgoing ACK) */
    while (HWREG(RFCORE_XREG_FSMSTAT1) & RFCORE_XREG_FSMSTAT1_TX_ACTIVE);
   
    /* Flush the RX and TX buffers */
    CC2538_RF_CSP_ISFLUSHRX();
    CC2538_RF_CSP_ISFLUSHTX();

    /* Don't turn off if we are off since this will trigger a Strobe Error */
    if (HWREG(RFCORE_XREG_RXENABLE) != 0)
    {
        CC2538_RF_CSP_ISRFOFF();
    }
}

void Radio::transmit(void)
{   
    /* Make sure we are not transmitting already */
    while(HWREG(RFCORE_XREG_FSMSTAT1) & RFCORE_XREG_FSMSTAT1_TX_ACTIVE)
        ;

    /* Enable radio interrupts */
    enableInterrupts();

    /* Enable transmit mode */
    CC2538_RF_CSP_ISTXON();

    /* Busy-wait until radio really transmitting */
    while(!((HWREG(RFCORE_XREG_FSMSTAT1) & RFCORE_XREG_FSMSTAT1_TX_ACTIVE)))
        ;
}

void Radio::receive(void)
{
    /* Flush the RX buffer */
    CC2538_RF_CSP_ISFLUSHRX();
 
    /* Enable radio interrupts */
    enableInterrupts();
   
    /* Enable receive mode */
    CC2538_RF_CSP_ISRXON();
   
    /* Busy-wait until radio really listening */
    while(!((HWREG(RFCORE_XREG_FSMSTAT1) & RFCORE_XREG_FSMSTAT1_RX_ACTIVE)))
        ;
}

void Radio::loadPacket(uint8_t* data, uint8_t length)
{
    /* Make sure previous transmission is not still in progress */
    while (HWREG(RFCORE_XREG_FSMSTAT1) & RFCORE_XREG_FSMSTAT1_TX_ACTIVE)
        ;
   
    /* Flush the TX buffer */
    CC2538_RF_CSP_ISFLUSHTX();
   
    /* Append the PHY length byte first */
    HWREG(RFCORE_SFR_RFDATA) = length;
   
    /* Append the packet payload, CRC length is included */
    for (uint8_t i = 0; i < length; i++)
    {
        HWREG(RFCORE_SFR_RFDATA) = data[i];
    }
}

/**
 * When reading the packet from the RX buffer, you get the following:
 * - *[1B]     Length
 * -  [0-125B] Payload (excluding CRC)
 * - *[2B]     CRC
 * - Notice in sniffer mode the CRC field get replaced by:
 * - *[1B]     RSSI (signed 2s complement)
 * - *[1B]     CRC_OK (1 bit) + Correlation (7 bits)
 */
uint8_t Radio::getPacket(uint8_t* buffer, uint8_t length, int8_t* rssi, uint8_t* crc)
{
   uint8_t crcValue;
   uint8_t packetLength;
   
   /* Check the packet length (first byte) */
   packetLength = HWREG(RFCORE_SFR_RFDATA);
   
   /* Check if packet is too long or too short */
   if ((packetLength > CC2538_RF_MAX_PACKET_LEN) || 
       (packetLength <= CC2538_RF_MIN_PACKET_LEN))
   {
      CC2538_RF_CSP_ISFLUSHRX();
      return 0;
   }
   
   /* Check if the packet fits to the buffer */
   if (packetLength > length)
   {
      CC2538_RF_CSP_ISFLUSHRX();
      return 0;
   }
   
   /* Copy the packet to the buffer except for the CRC (last two bytes) */
   for (uint8_t i = 0; i < (packetLength - 2); i++)
   {
      buffer[i] = HWREG(RFCORE_SFR_RFDATA);
   }
   
   *rssi      = ((int8_t) (HWREG(RFCORE_SFR_RFDATA)) - CC2538_RF_RSSI_OFFSET);
   crcValue   = HWREG(RFCORE_SFR_RFDATA);
   *crc       = crcValue & CC2538_RF_CRC_BITMASK;
   
   /* Flush the RX buffer */
   CC2538_RF_CSP_ISFLUSHRX();
   
   return packetLength;
}

void Radio::registerRxCallbacks(Callback* rxInit_, Callback* rxDone_)
{
    rxInit = rxInit_;
    rxDone = rxDone_;
}

void Radio::registerTxCallbacks(Callback* txInit_, Callback* txDone_)
{
    txInit = txInit_;
    txDone = txDone_;
}

void Radio::setChannel(uint8_t channel)
{
    // Check that the channel is within bounds
    if (!(channel < CC2538_RF_CHANNEL_MIN) || !(channel > CC2538_RF_CHANNEL_MAX))
    {
        /* Changes to FREQCTRL take effect after the next recalibration */
        HWREG(RFCORE_XREG_FREQCTRL) = (CC2538_RF_CHANNEL_MIN +
                                      (channel - CC2538_RF_CHANNEL_MIN) * CC2538_RF_CHANNEL_SPACING);
    }
}

void Radio::setPower(uint8_t power)
{
    HWREG(RFCORE_XREG_TXPOWER) = power;
}

/*=============================== protected =================================*/

void Radio::interruptHandler(void)
{
    uint32_t irq_status0, irq_status1;

    /* Read RFCORE_STATUS */
    irq_status0 = HWREG(RFCORE_SFR_RFIRQF0);
    irq_status1 = HWREG(RFCORE_SFR_RFIRQF1);

    /* Clear pending interrupt */
    IntPendClear(INT_RFCORERTX);

    /* Clear RFCORE_STATUS */
    HWREG(RFCORE_SFR_RFIRQF0) = 0;
    HWREG(RFCORE_SFR_RFIRQF1) = 0;

    /* STATUS0 Register: Start of frame event */
    if ((irq_status0 & RFCORE_SFR_RFIRQF0_SFD) == RFCORE_SFR_RFIRQF0_SFD)
    {
        // txInit
        // rxInit
    }

    /* STATUS0 Register: End of frame event */
    if (((irq_status0 & RFCORE_SFR_RFIRQF0_RXPKTDONE) ==  RFCORE_SFR_RFIRQF0_RXPKTDONE))
    {
        // rxDone
    }

    /* STATUS0 Register: Fifo is full event */
    if (((irq_status0 & RFCORE_SFR_RFIRQF0_FIFOP) ==  RFCORE_SFR_RFIRQF0_FIFOP))
    {
        //
    }

    /* STATUS1 Register: End of frame event */
    if (((irq_status1 & RFCORE_SFR_RFIRQF1_TXDONE) == RFCORE_SFR_RFIRQF1_TXDONE))
    {
        // txDone
    }
}

void Radio::errorHandler(void)
{
    uint32_t rferrm;

    /* Read RFERR_STATUS */
    rferrm = HWREG(RFCORE_XREG_RFERRM);

    /* Clear pending interrupt */
    IntPendClear(INT_RFCOREERR);

    /*  */
    if ((HWREG(RFCORE_XREG_RFERRM) & (((0x02) << RFCORE_XREG_RFERRM_RFERRM_S) & RFCORE_XREG_RFERRM_RFERRM_M)) & ((uint32_t) rferrm))
    {
        //
        HWREG(RFCORE_XREG_RFERRM) = ~(((0x02)<<RFCORE_XREG_RFERRM_RFERRM_S)&RFCORE_XREG_RFERRM_RFERRM_M);

        // ToDo: Handler error
    }
    else
    {
        while(true);
    }
}

/*================================ private ==================================*/

void Radio::on(void)
{
    CC2538_RF_CSP_ISRXON();
}

void Radio::off(void)
{
    /* Wait for ongoing TX to complete (e.g. this could be an outgoing ACK) */
    while (HWREG(RFCORE_XREG_FSMSTAT1) & RFCORE_XREG_FSMSTAT1_TX_ACTIVE)
        ;

    /* Don't turn off if we are off as this will trigger a Strobe Error */
    if (HWREG(RFCORE_XREG_RXENABLE) != 0)
    {
        /*  */
        CC2538_RF_CSP_ISRFOFF();
      
       /* Clear FIFO interrupt flags */
       HWREG(RFCORE_SFR_RFIRQF0) = ~(RFCORE_SFR_RFIRQF0_FIFOP|RFCORE_SFR_RFIRQF0_RXPKTDONE);
   }
}

void Radio::enableInterrupts(void)
{
    /* Enable RF interrupts 0, RXPKTDONE, SFD and FIFOP only -- see page 751  */
    HWREG(RFCORE_XREG_RFIRQM0) |= ((0x06|0x02|0x01) << RFCORE_XREG_RFIRQM0_RFIRQM_S) & RFCORE_XREG_RFIRQM0_RFIRQM_M;

    /* Enable RF interrupts 1, TXDONE only */
    HWREG(RFCORE_XREG_RFIRQM1) |= ((0x02) << RFCORE_XREG_RFIRQM1_RFIRQM_S) & RFCORE_XREG_RFIRQM1_RFIRQM_M;
    
    /* Enable RF error interrupts */
    HWREG(RFCORE_XREG_RFERRM) = RFCORE_XREG_RFERRM_RFERRM_M;
    
    /* Enable transmit, receive and error interrupts */
    IntEnable(INT_RFCORERTX);
    IntEnable(INT_RFCOREERR);
}

void Radio::disableInterrupts(void)
{
    /* Enable RF interrupts 0, RXPKTDONE,SFD,FIFOP only -- see page 751  */
    HWREG(RFCORE_XREG_RFIRQM0) = 0;

    /* Enable RF interrupts 1, TXDONE only */
    HWREG(RFCORE_XREG_RFIRQM1) = 0;
}
