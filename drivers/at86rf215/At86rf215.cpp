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

#include "At86rf215.hpp"
#include "At86rf215_regs.h"

#include "BoardImplementation.hpp"

#include "platform_types.h"

/*================================ define ===================================*/

#define AT86RF215_RSSI_MAX_VALUE        ( 127 )

#define AT86RF215_ED_MIN_VALUE          ( -127 )
#define AT86RF215_ED_MAX_VALUE          ( 4 )

#define AT86RF215_DELAY_MS              ( 1 )

#define AT86RF215_RFn_IRQM              ( 0x00 )
#define AT86RF215_BBCn_IRQM             ( 0x13 )

#define AT86RF215_RFn_PAC_PACUR_MASK    ( 0x60 )
#define AT86RF215_BBCn_PC_CTX_MASK      ( 0x80 )
#define AT86RF215_BBCn_PC_FCST_MASK     ( 0x08 )

/*================================ typedef ==================================*/

/*=============================== variables =================================*/

extern BoardImplementation board;

/*=============================== prototypes ================================*/

/*================================= public ==================================*/

At86rf215::At86rf215(Spi& spi, GpioOut& pwr, GpioOut& rst, GpioOut& cs, GpioIn& irq):
  spi_(spi), pwr_(pwr), rst_(rst), cs_(cs), irq_(irq),
  callback_(this, &At86rf215::interruptHandler)

{
  irq_.setCallback(&callback_);
  cs_.low();
  rst_.low();
  pwr_.low();
  board.delayMilliseconds(AT86RF215_DELAY_MS);
}

void At86rf215::on(void)
{
  pwr_.high();
  board.delayMilliseconds(AT86RF215_DELAY_MS);
  cs_.high();
  rst_.high();
  board.delayMilliseconds(AT86RF215_DELAY_MS);
}

void At86rf215::off(void)
{
  cs_.low();
  rst_.low();
  pwr_.low();
  board.delayMilliseconds(AT86RF215_DELAY_MS);
}

void At86rf215::hardReset(void)
{
  rst_.high();
  board.delayMilliseconds(AT86RF215_DELAY_MS);
  rst_.low();
  board.delayMilliseconds(AT86RF215_DELAY_MS);
}

void At86rf215::softReset(RadioCore rc)
{
    goToState(rc, RadioCommand::CMD_RESET, RadioState::STATE_RESET);
}

bool At86rf215::check(void)
{
  bool status = false;

  uint8_t pn = 0;
  uint8_t ver = 0;

  singleAccessRead(RF_PN, &pn);

  if (pn == PN_215)
  {
    singleAccessRead(RF_VN, &ver);

    if (ver == VN_1_1 || ver == VN_1_3)
    {
      status = true;
    }
  }

  return status;
}

void At86rf215::configure(RadioCore rc, const radio_settings_t* radio_settings, const frequency_settings_t* frequency_settings)
{
  uint16_t rf_base, bbc_base;
  uint16_t address;
  uint8_t value;

  register_t irq_settings[] =
  {
    {RF09_BASE + RFn_IRQM,  AT86RF215_RFn_IRQM},
    {BBC0_BASE + BBCn_IRQM, AT86RF215_BBCn_IRQM},
    {RF24_BASE + RFn_IRQM,  AT86RF215_RFn_IRQM},
    {BBC1_BASE + BBCn_IRQM, AT86RF215_BBCn_IRQM},
  };
  uint8_t* irq_mask[] = {&rf09_irqm, &bbc0_irqm, &rf24_irqm, &bbc1_irqm};
  
  register_t channel_settings[] = 
  {
    {RFn_CCF0L, (uint8_t) ((frequency_settings->frequency0 / 25) % 256)},
    {RFn_CCF0H, (uint8_t) ((frequency_settings->frequency0 / 25) / 256)},
    {RFn_CS,    (uint8_t) (frequency_settings->channel_spacing / 25)},
    {RFn_CNL,   (uint8_t) (frequency_settings->channel % 256)},
    {RFn_CNM,   (uint8_t) (frequency_settings->channel / 256)},
  };

  /* Get RFn and BBCn base address based on radio core */
  rf_base  = getRFRegisterAddress(rc, 0x00);
  bbc_base = getFBRegisterAddress(rc, 0x00);

  /* Configure default settings */
  for (uint16_t i = 0; i < sizeof(irq_settings)/sizeof(irq_settings[0]); i++)
  {
    /* Calculate register address and set value */
    address = irq_settings[i].address;
    value   = irq_settings[i].value;
    
    /* Write default IRQ settings to radio */
    singleAccessWrite(address, value);
    
    /* Store IRQ settings */
    *irq_mask[i] = value;
  }

  /* Configure RF settings */
  for (uint16_t i = 0; i < radio_settings->rf_elements; i++)
  {
    /* Calculate register address and set value */
    address = rf_base + radio_settings->rf_registers[i].address;
    value   = radio_settings->rf_registers[i].value;

    /* Write RF settings to radio */
    singleAccessWrite(address, value); 
  }

  /* Configure BBC settings */
  for (uint16_t i = 0; i < radio_settings->bbc_elements; i++)
  {
    /* Calculate register address and set value */
    address = bbc_base + radio_settings->bbc_registers[i].address;
    value   = radio_settings->bbc_registers[i].value;

    /* Write BBC settings to radio */
    singleAccessWrite(address, value); 
  }

  /* Configure channel settings */
  for (uint16_t i = 0; i < sizeof(channel_settings)/sizeof(channel_settings[0]); i++)
  {
    /* Calculate register address and set value */
    address = rf_base + channel_settings[i].address;
    value   = channel_settings[i].value;
    
    /* Write channel settings to radio */
    singleAccessWrite(address, value);
  }
  
  /* Get CRC length based on radio configuration */
  crc_length = getCRCLength(rc);
}

void At86rf215::wakeup(RadioCore rc)
{
    goToState(rc, RadioCommand::CMD_TRXOFF, RadioState::STATE_TRXOFF);
}

void At86rf215::ready(RadioCore rc)
{
    goToState(rc, RadioCommand::CMD_TXPREP, RadioState::STATE_TXPREP);
}

void At86rf215::transmit(RadioCore rc)
{
  goToState(rc, RadioCommand::CMD_TX, RadioState::STATE_TX);
  
  if (rc == CORE_RF09 && tx09Init_ != nullptr)
  {
    tx09Init_->execute();
  } 
  else if (rc == CORE_RF24 && tx24Init_ != nullptr)
  {
    tx24Init_->execute();
  }
}

void At86rf215::receive(RadioCore rc)
{
  goToState(rc, RadioCommand::CMD_RX, RadioState::STATE_RX);
}

void At86rf215::setRxCallbacks(RadioCore rc, Callback* rxInit, Callback* rxDone)
{
  if (rc == RadioCore::CORE_RF09)
  {
    rx09Init_ = rxInit;
    rx09Done_ = rxDone;
  } 
  else if  (rc == RadioCore::CORE_RF24)
  {
    rx24Init_ = rxInit;
    rx24Done_ = rxDone;
  }
}

void At86rf215::setTxCallbacks(RadioCore rc, Callback* txInit, Callback* txDone)
{
  if (rc == RadioCore::CORE_RF09)
  {
    tx09Init_ = txInit;
    tx09Done_ = txDone;
  } 
  else if  (rc == RadioCore::CORE_RF24)
  {
    tx24Init_ = txInit;
    tx24Done_ = txDone;
  }
}

void At86rf215::enableInterrupts(void)
{
  irq_.enableInterrupts();
}

void At86rf215::disableInterrupts(void)
{
  irq_.disableInterrupts();
}

At86rf215::RadioResult At86rf215::getRSSI(RadioCore rc, int8_t* rssi)
{
  uint16_t address;
    int8_t value;
  
  /* Select RSSI register address */
  address = getRFRegisterAddress(rc, RFn_RSSI);
  
  /* Read RSSI value */
  singleAccessRead(address, (uint8_t *)&value);

  /* Check that the RSSI value is valid */
  if (value == AT86RF215_RSSI_MAX_VALUE)
  {
    *rssi = 0;
    return RadioResult::Error;
  }
  else
  {
    *rssi = value;
    return RadioResult::Success;
  }
}

At86rf215::RadioResult At86rf215::getED(RadioCore rc, int8_t* ed)
{
  uint16_t address;
  int8_t value;

  /* Select ED register address */
  address = getRFRegisterAddress(rc, RFn_EDV);
  
  /* Read ED value */
  singleAccessRead(address, (uint8_t *)&value);

  /* Check that ED value is valid */
  if (value < AT86RF215_ED_MIN_VALUE || value > AT86RF215_ED_MAX_VALUE)
  {
    *ed = 0;
    return RadioResult::Error;
  }
  else
  {
    *ed = value;
    return RadioResult::Success;
  }
}

At86rf215::RadioResult At86rf215::setTransmitPower(RadioCore rc, TransmitPower power)
{
  uint16_t address;
  int8_t value;
  
  /* Select PAC register address */
  address = getRFRegisterAddress(rc, RFn_PAC);
  
  /* Read PAC register */
  singleAccessRead(address, (uint8_t *)&value);
  
  /* Mask current control bits and set transmit power bits */
  value = (value & AT86RF215_RFn_PAC_PACUR_MASK) | power;
  
  /* Write PAC register */
  singleAccessWrite(address, value);
  
  return RadioResult::Success;
}

At86rf215::RadioResult At86rf215::loadPacket(RadioCore rc, uint8_t* data, uint16_t length)
{
  uint16_t bbc_txfll;
  uint16_t bbc_fbtxs;
  uint8_t scratch[2];
  
  /* Account for CRC length */
  length += crc_length;
  
  /* Get BBC and FB register address */
  bbc_txfll = getBBCRegisterAddress(rc, BBCn_TXFLL);
  bbc_fbtxs = getFBRegisterAddress(rc, BBCn_FBTXS);
  
  /* Set packet length */
  scratch[0] = (uint8_t)((length >> 0) & 0xFF); /* low byte */
  scratch[1] = (uint8_t)((length >> 8) & 0x07); /* high byte */
   
  /* Write the packet length */
  blockAccessWrite(bbc_txfll, scratch, 2);
  
  /* Send the packet payload */
  blockAccessWrite(bbc_fbtxs, data, length);
  
  return RadioResult::Success;
}

At86rf215::RadioResult At86rf215::getPacket(RadioCore rc, uint8_t* buffer, uint16_t* length, int8_t* rssi, int8_t* lqi, bool* crc)
{
  uint16_t bbc_rxfll, bbc_pc;
  uint16_t rf_rssi, rf_edv;
  uint16_t bbc_fbrxs;
  uint16_t scratch;
  uint8_t scratch_buffer[2];
  uint8_t byte;
  
  /* Get BBC, RF and FB register address */
  bbc_rxfll = getBBCRegisterAddress(rc, BBCn_RXFLL);
  bbc_pc    = getBBCRegisterAddress(rc, BBCn_PC);
  rf_rssi   = getRFRegisterAddress(rc, RFn_RSSI);
  rf_edv    = getRFRegisterAddress(rc, RFn_EDV);
  bbc_fbrxs = getFBRegisterAddress(rc, BBCn_FBRXS);
  
  /* Read the packet packet length */
  blockAccessRead(bbc_rxfll, scratch_buffer, 2);
  
  /* Compute packet length */
  scratch  = (scratch_buffer[0] << 0);
  scratch |= (scratch_buffer[1] << 8);
  
  /* Account for CRC length */
  scratch -= crc_length;
  
  /* Avoid overflowing the buffer */
  if (scratch > *length)
  {
    return RadioResult::Error;
  }
  else
  {
    /* Update packet length */
    *length = scratch;
  }
  
  /* Read the packet payload */
  blockAccessRead(bbc_fbrxs, buffer, scratch);
  
  /* Read packet RSSI */
  singleAccessRead(rf_rssi, &byte);
  *rssi = byte;

  /* Read packet LQI */
  singleAccessRead(rf_edv, &byte);
  *lqi = byte;
  
  /* Read packet CRC */
  singleAccessRead(bbc_pc, &byte);
  *crc = (bool)( byte >> 5);
  
  return RadioResult::Success;
}

void At86rf215::setContinuousTransmission(RadioCore rc, bool enable)
{
  uint16_t bbc_pc;
  uint8_t value;
  
  /* Get BBC register address */
  bbc_pc = getBBCRegisterAddress(rc, BBCn_PC); 
  
  /* Read BBCn_PC register */
  singleAccessRead(bbc_pc, &value);
  
  /* Enable or disable the continuous transmission bit */
  if (enable)
  {
    value |= AT86RF215_BBCn_PC_CTX_MASK;
  }
  else
  {
    value ^= AT86RF215_BBCn_PC_CTX_MASK;
  }
  
  /* Write BBCn_PC register */
  singleAccessWrite(bbc_pc, value);
}

/*=============================== protected =================================*/

void At86rf215::interruptHandler(void)
{
  /* Read interrupt registers */
  uint8_t rf09_irqs, rf24_irqs;
  uint8_t bbc0_irqs, bbc1_irqs;

  /* Read RF and BBC interrupt status */
  singleAccessRead(RF09_IRQS, &rf09_irqs);
  singleAccessRead(RF24_IRQS, &rf24_irqs);
  singleAccessRead(BBC0_IRQS, &bbc0_irqs);
  singleAccessRead(BBC1_IRQS, &bbc1_irqs);

  /* Call RF09 interrupt handler */
  if ((rf09_irqs & rf09_irqm) || (bbc0_irqs & bbc0_irqm))
  {
    interruptHandler_rf09(rf09_irqs, bbc0_irqs);
  }

  /* Call RF24 interrupt handler */
  if ((rf24_irqs & rf24_irqm) || (bbc1_irqs & bbc1_irqm))
  {
    interruptHandler_rf24(rf24_irqs, bbc1_irqs);
  }
}

/**
 * RFn_IRQS (Radio status interrupt, pag. 40)
 * IQIFSF: 
 * TRXERR:
 * BATLOW
 * EDC
 * TRXRDY: Transceiver ready interrupt 
 * WAKEUP: Wake-up/reset interrupt
 */

/**
 * BBCn_IRQS (Baseband status interrupt, pag. 41)
 * FBLI: Frame Buffer Level indicator
 * AGCR: AGC release interrupt
 * AGCH: AGC Hold interrupt
 * TXFE: Transmitter Frame End interrupt
 * RXEM: Receiver Extended Match interrupt
 * RXAM: Receiver Address Match interrupt
 * RXFE: Receiver Frame End interrupt
 * RXFS: Receiver Frame Start interrupt
 */

void At86rf215::interruptHandler_rf09(uint8_t rf_irqs, uint8_t bbc_irqs)
{
  /* Receiver start of frame */
  if ((bbc_irqs & 0x01) && (rx09Init_ != nullptr))
  {
    rx09Init_->execute();
  }

  /* Receiver end of frame */
  if ((bbc_irqs & 0x02) && (rx09Done_ != nullptr))
  {
    rx09Done_->execute();
  }

  /* Transmitter start of frame */
  if (tx09Init_ != nullptr)
  {
    /* tx09Init_->execute(); */
  }

  /* Transmitter end of frame */
  if ((bbc_irqs & 0x10) && (tx09Done_ != nullptr))
  {
    tx09Done_->execute();
  }
}

void At86rf215::interruptHandler_rf24(uint8_t rf_irqs, uint8_t bbc_irqs)
{
  /* Receiver start of frame */
  if ((bbc_irqs & 0x01) && (rx24Init_ != nullptr))
  {
    rx24Init_->execute();
  }

  /* Receiver end of frame */
  if ((bbc_irqs & 0x02) && (rx24Done_ != nullptr))
  {
    rx24Done_->execute();
  }
  
  /* Transmitter start of frame */
  if (tx24Init_ != nullptr)
  {
    /* tx24Init_->execute(); */
  }

  /* Transmitter end of frame */
  if ((bbc_irqs & 0x10) && (tx24Done_ != nullptr))
  {
    tx24Done_->execute();
  }
}

/*================================ private ==================================*/

At86rf215::RadioState At86rf215::getState(RadioCore rc)
{
  RadioState state;
  uint16_t address;
  
  /* Select registers to use */
  address = getRFRegisterAddress(rc, RFn_STATE);
  
  /* Read radio state */
  singleAccessRead(address, (uint8_t*) &state); 

  return state;
}

void At86rf215::goToState(RadioCore rc, RadioCommand cmd, RadioState target)
{
  RadioState current;

  /* Write command to radio core */
  writeCmd(rc, cmd);

  /* Repeat until the core is at target state */
  do {
    /* Read radio core state */
    current = getState(rc);
    if (target != current)
    {
      board.delayMicroseconds(100);
    }
  } while (target != current);
}

void At86rf215::writeCmd(RadioCore rc, RadioCommand cmd)
{
  uint16_t address;
  
  /* Select registers to use */
  address = getRFRegisterAddress(rc, RFn_CMD);
  
  /* Write radio command */
  singleAccessWrite(address, (uint8_t) cmd);
}

void At86rf215::singleAccessRead(uint16_t address, uint8_t* value)
{
  uint8_t spi_tx_transaction[3];
  uint8_t spi_rx_transaction[3];
  uint8_t address_hi, address_lo;
     
  /* Prepare device address */
  address_hi = (uint8_t) ((address & 0xFF00) >> 8);
  address_lo = (uint8_t) ((address & 0x00FF) >> 0);

  /* Prepare buffer for SPI transaction */
  spi_tx_transaction[0] = READ_CMD | address_hi;
  spi_tx_transaction[1] = address_lo;
  spi_tx_transaction[2] = 0x00;

  /* Activate CS */
  cs_.low();

  /* Write the SPI transaction */
  spi_.rwByteDma(spi_tx_transaction, 3, spi_rx_transaction, 3);

  /* Deactivate CS */
  cs_.high();

  /* Return transaction value */
  *value = spi_rx_transaction[2];
}

void At86rf215::singleAccessWrite(uint16_t address, uint8_t value)
{
  uint8_t spi_tx_transaction[3];
  uint8_t spi_rx_transaction[3];
  uint8_t address_hi, address_lo;

  /* Prepare device address */
  address_hi = (uint8_t) ((address & 0xFF00) >> 8);
  address_lo = (uint8_t) ((address & 0x00FF) >> 0);

  /* Prepare buffer for SPI transaction */
  spi_tx_transaction[0] = WRITE_CMD | address_hi;
  spi_tx_transaction[1] = address_lo;
  spi_tx_transaction[2] = value;

  /* Activate CS */
  cs_.low();

  /* Write the SPI transaction */
  spi_.rwByteDma(spi_tx_transaction, 3, spi_rx_transaction, 3);

  /* Deactivate CS */
  cs_.high();
}

void At86rf215::blockAccessRead(uint16_t address, uint8_t* values, uint16_t length)
{
  uint8_t spi_tx_transaction[2];
  uint8_t spi_rx_transaction[2];
  uint8_t address_hi, address_lo;

  /* Prepare device address */
  address_hi = (uint8_t) ((address & 0xFF00) >> 8);
  address_lo = (uint8_t) ((address & 0x00FF) >> 0);

  /* Prepare buffer for SPI transaction */
  spi_tx_transaction[0] = READ_CMD | address_hi;
  spi_tx_transaction[1] = address_lo;

  /* Activate CS */
  cs_.low();
  
  /* Send device address */
  spi_.rwByteDma(spi_tx_transaction, 2, spi_rx_transaction, 2);
  
  /* Receive device bytes */
  spi_.rwByteDma(NULL, 0, values, length);

  /* Deactivate CS */
  cs_.high();
}

void At86rf215::blockAccessWrite(uint16_t address, uint8_t* values, uint16_t length)
{
  uint8_t spi_tx_transaction[2];
  uint8_t spi_rx_transaction[2];
  uint8_t address_hi, address_lo;

  /* Prepare device address */
  address_hi = (uint8_t) ((address & 0xFF00) >> 8);
  address_lo = (uint8_t) ((address & 0x00FF) >> 0);

  /* Prepare buffer for SPI transaction */
  spi_tx_transaction[0] = WRITE_CMD | address_hi;
  spi_tx_transaction[1] = address_lo;

  /* Activate CS */
  cs_.low();
  
  /* Send device address */
  spi_.rwByteDma(spi_tx_transaction, 2, spi_rx_transaction, 2);
  
  /* Send payload */
  spi_.rwByteDma(values, length, NULL, 0);
  
  /* Deactivate CS */
  cs_.high();
}

inline uint16_t At86rf215::getRFRegisterAddress(RadioCore rc, uint16_t address)
{
  /* Select RF register address based on radio core */
  switch(rc)
  {
    case RadioCore::CORE_RF09:
      address = RF09_BASE + address;
      break;
    case RadioCore::CORE_RF24:
      address = RF24_BASE + address;
      break;
    default:
      return 0;
      break;
  }
  
  return address;
}

inline uint16_t At86rf215::getBBCRegisterAddress(RadioCore rc, uint16_t address)
{
  /* Select BBC register address based on radio core */
  switch(rc)
  {
    case RadioCore::CORE_RF09:
      address = BBC0_BASE + address;
      break;
    case RadioCore::CORE_RF24:
      address = BBC1_BASE + address;
      break;
    default:
      return 0;
      break;
  }
  
  return address;
}

inline uint16_t At86rf215::getFBRegisterAddress(RadioCore rc, uint16_t address)
{
  /* Select FB register address based on radio core */
  switch(rc)
  {
    case RadioCore::CORE_RF09:
      address = BBC0_FB_BASE + address;
      break;
    case RadioCore::CORE_RF24:
      address = BBC1_FB_BASE + address;
      break;
    default:
      return 0;
      break;
  }
  
  return address;
}

inline uint16_t At86rf215::getCRCLength(RadioCore rc)
{
  uint16_t address;
  uint8_t value;
  
  /* Get BBC register address */
  address = getBBCRegisterAddress(rc, BBCn_PC);
  
  /* Read BBCn_PC register */
  singleAccessRead(address, &value);
  
  /* If FCST is active CRC = 16 bits, otherwise CRC = 32 bits */
  if ((value & AT86RF215_BBCn_PC_FCST_MASK) == AT86RF215_BBCn_PC_FCST_MASK)
  {
    return 2;
  }
  else
  {
    return 4;
  }
}
