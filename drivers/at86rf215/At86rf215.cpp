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

#define AT86RF215_RSSI_MAX_VALUE    ( 127 )

#define AT86RF215_ED_MIN_VALUE      ( -127 )
#define AT86RF215_ED_MAX_VALUE      ( 4 )

#define AT86RF215_DELAY_MS          ( 1 )

/*================================ typedef ==================================*/

/*=============================== variables =================================*/

extern BoardImplementation board;

uint8_t rf09_irqm, rf24_irqm;
uint8_t bbc0_irqm, bbc1_irqm;

/*=============================== prototypes ================================*/

/*================================= public ==================================*/

At86rf215::At86rf215(Spi& spi, GpioOut& pwr, GpioOut& rst, GpioOut& cs, GpioIn& irq):
  spi_(spi), pwr_(pwr), rst_(rst), cs_(cs), irq_(irq),
  callback_(this, &At86rf215::interruptHandler)

{
  irq_.setCallback(&callback_);
  cs_.high();
  rst_.high();
  pwr_.low();
}

void At86rf215::on(void)
{
  cs_.high();
  rst_.low();
  board.delayMilliseconds(AT86RF215_DELAY_MS);
  pwr_.high();
  board.delayMilliseconds(AT86RF215_DELAY_MS);
}

void At86rf215::off(void)
{
  cs_.high();
  rst_.high();
  board.delayMilliseconds(AT86RF215_DELAY_MS);
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

void At86rf215::configure(void)
{
	rf09_irqm = 0x00;
  singleAccessWrite(AT86RF215_RF09_IRQM_ADDR, rf09_irqm);

  rf24_irqm = 0x00;
  singleAccessWrite(AT86RF215_RF24_IRQM_ADDR, rf24_irqm);

  bbc0_irqm = AT86RF215_BBCn_IRQM_TXFE_(1) | AT86RF215_BBCn_IRQM_RXFE_(1) | AT86RF215_BBCn_IRQM_RXFS_(1);
  singleAccessWrite(AT86RF215_BBC0_IRQM_ADDR, bbc0_irqm);

  bbc1_irqm = AT86RF215_BBCn_IRQM_TXFE_(1) | AT86RF215_BBCn_IRQM_RXFE_(1) | AT86RF215_BBCn_IRQM_RXFS_(1);
  singleAccessWrite(AT86RF215_BBC1_IRQM_ADDR, bbc1_irqm);
}

void At86rf215::sleep(RadioCore rc)
{
	goToState(rc, RadioCommand::CMD_SLEEP, RadioState::STATE_SLEEP);
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
}

void At86rf215::receive(RadioCore rc)
{
	goToState(rc, RadioCommand::CMD_RX, RadioState::STATE_RX);
}

bool At86rf215::check(void)
{
  bool status = false;

  uint8_t pn;
  uint8_t ver;

  singleAccessRead(AT86RF215_PN_ADDR, &pn);

  if (pn == AT86RF215_PN_215)
  {
    singleAccessRead(AT86RF215_VN_ADDR, &ver);

    if (ver == AT86RF215_VN_1_1 || ver == AT86RF215_VN_1_3)
    {
      status = true;
    }
  }

  return status;
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

bool At86rf215::getRSSI(RadioCore rc, int8_t* rssi)
{
	int8_t value;

	if (rc == RadioCore::CORE_RF09)
	{
		singleAccessRead(AT86RF215_RF09_RSSI_ADDR, (uint8_t *)&value);
	} 
  else if (rc == RadioCore::CORE_RF24)
	{
		singleAccessRead(AT86RF215_RF24_RSSI_ADDR, (uint8_t *)&value);
	}
  else
  {
		return false;
	}

	if (value == AT86RF215_RSSI_MAX_VALUE)
	{
		*rssi = 0;
		return false;
	}
	else
	{
		*rssi = value;
		return true;
	}
}

bool At86rf215::getED(RadioCore rc, int8_t* ed)
{
	int8_t value;

	if (rc == RadioCore::CORE_RF09)
	{
		singleAccessRead(AT86RF215_RF09_EDV_ADDR, (uint8_t *)&value);
	} 
  else if (rc == RadioCore::CORE_RF24)
	{
		singleAccessRead(AT86RF215_RF24_EDV_ADDR, (uint8_t *)&value);
	} 
  else
  {
		return false;
	}

	if (value < AT86RF215_ED_MIN_VALUE || value > AT86RF215_ED_MAX_VALUE)
	{
		*ed = 0;
		return false;
	}
	else
	{
		*ed = value;
		return true;
	}
}

At86rf215::RadioResult At86rf215::loadPacket(RadioCore rc, uint8_t* data, uint8_t length)
{
	return RadioResult::Error;
}

At86rf215::RadioResult At86rf215::getPacket(RadioCore rc, uint8_t* buffer, uint8_t* length, int8_t* rssi, uint8_t* lqi, uint8_t* crc)
{
	return RadioResult::Error;
}

/*=============================== protected =================================*/

void At86rf215::interruptHandler(void)
{
  /* Read interrupt registers */
  uint8_t rf09_irqs, rf24_irqs;
  uint8_t bbc0_irqs, bbc1_irqs;

  /* Read RF and BBC interrupt status */
  singleAccessRead(AT86RF215_RF09_IRQS_ADDR, &rf09_irqs);
  singleAccessRead(AT86RF215_RF24_IRQS_ADDR, &rf24_irqs);
  singleAccessRead(AT86RF215_BBC0_IRQS_ADDR, &bbc0_irqs);
  singleAccessRead(AT86RF215_BBC1_IRQS_ADDR, &bbc1_irqs);

  /* Call RF09 interrupt handler */
  if (rf09_irqs & rf09_irqm || bbc0_irqs & bbc0_irqm)
  {
    interruptHandler_rf09(rf09_irqs, bbc0_irqs);
  }

/* Call RF24 interrupt handler */
  if (rf24_irqs & rf24_irqm || bbc1_irqs & bbc1_irqm)
  {
    interruptHandler_rf24(rf24_irqs, bbc1_irqs);
  }
}

void At86rf215::interruptHandler_rf09(uint8_t rf_irqs, uint8_t bbc_irqs)
{
	/* Receiver start of frame */
	if (bbc_irqs & AT86RF215_BBCn_IRQM_RXFS_(1) && rx09Init_ != nullptr)
  {
    rx09Init_->execute();
  }

	/* Receiver end of frame */
  if (bbc_irqs & AT86RF215_BBCn_IRQM_RXFE_(1) && rx09Done_ != nullptr)
  {
    rx09Done_->execute();
  }

  if (tx09Init_ != nullptr)
  {
    /* tx09Init_->execute(); */
  }

  /* Transmitter end of frame */
  if (bbc_irqs & AT86RF215_BBCn_IRQM_TXFE_(1) &&
    tx09Done_ != nullptr)
  {
    tx09Done_->execute();
  }
}

void At86rf215::interruptHandler_rf24(uint8_t rf_irqs, uint8_t bbc_irqs)
{
	/* Receiver start of frame */
	if (bbc_irqs & AT86RF215_BBCn_IRQM_RXFS_(1) && rx24Init_ != nullptr)
  {
    rx24Init_->execute();
  }

	/* Receiver end of frame */
  if (bbc_irqs & AT86RF215_BBCn_IRQM_RXFE_(1) && rx24Done_ != nullptr)
  {
    rx24Done_->execute();
  }

  if (tx24Init_ != nullptr)
  {
    /* tx24Init_->execute(); */
  }

	/* Transmitter end of frame */
  if (bbc_irqs & AT86RF215_BBCn_IRQM_TXFE_(1) && tx24Done_ != nullptr)
  {
    tx24Done_->execute();
  }
}

/*================================ private ==================================*/

At86rf215::RadioState At86rf215::getState(RadioCore rc)
{
	RadioState state;

	if (rc == RadioCore::CORE_RF09)
	{
		singleAccessRead(AT86RF215_RF09_STATE_ADDR, (uint8_t*) &state);	
	} 
  else if (rc == RadioCore::CORE_RF24)
	{
		singleAccessRead(AT86RF215_RF24_STATE_ADDR, (uint8_t*) &state);	
	}

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
      board.delayMilliseconds(1);
    }
	} while (target != current);
}

void At86rf215::writeCmd(RadioCore rc, RadioCommand cmd)
{
	if (rc == RadioCore::CORE_RF09)
	{
		singleAccessWrite(AT86RF215_RF09_CMD_ADDR, (uint8_t) cmd);
	} 
  else if (rc == RadioCore::CORE_RF24)
	{
		singleAccessWrite(AT86RF215_RF24_CMD_ADDR, (uint8_t) cmd);	
	}
}

void At86rf215::singleAccessRead(uint16_t address, uint8_t* value)
{
  uint8_t spi_transaction[3];
  uint8_t address_hi, address_lo;
     
  /* Prepare device address */
  address_hi = (uint8_t) ((address & 0xFF00) >> 8);
  address_lo = (uint8_t) ((address & 0x00FF) >> 0);

  /* Prepare buffer for SPI transaction */
  spi_transaction[0] = AT86RF215_READ_CMD | address_hi;
  spi_transaction[1] = address_lo;
  spi_transaction[2] = 0x00;

  /* Activate CS */
  cs_.low();

  /* Write the SPI transaction */
  spi_.rwByte(spi_transaction, 3, spi_transaction, 3);

  /* Deactivate CS */
  cs_.high();

  /* Return transaction value */
  *value = spi_transaction[2];
}

void At86rf215::singleAccessWrite(uint16_t address, uint8_t value)
{
  uint8_t spi_transaction[3];
  uint8_t address_hi, address_lo;

  /* Prepare device address */
  address_hi = (uint8_t) ((address & 0xFF00) >> 8);
  address_lo = (uint8_t) ((address & 0x00FF) >> 0);

  /* Prepare buffer for SPI transaction */
  spi_transaction[0] = AT86RF215_WRITE_CMD | address_hi;
  spi_transaction[1] = address_lo;
  spi_transaction[2] = value;

  /* Activate CS */
  cs_.low();

  /* Write the SPI transaction */
  spi_.rwByte(spi_transaction, 3, spi_transaction, 3);

  /* Deactivate CS */
  cs_.high();
}

void At86rf215::blockAccessRead(uint16_t address, uint8_t* values, uint16_t length)
{
  uint8_t address_hi, address_lo;

  /* Prepare device address */
  address_hi = (uint8_t) ((address & 0xFF00) >> 8);
  address_lo = (uint8_t) ((address & 0x00FF) >> 0);

  /* Prepare buffer for SPI transaction */
  values[0] = AT86RF215_READ_CMD | address_hi;
  values[1] = address_lo;

  /* Activate CS */
  cs_.low();

  /* Repeat until completed */
  for (uint16_t i = 0; i < length; i++)
  {
    /* Read byte to the SPI interface */
    uint8_t b = spi_.rwByte(values[i]);
    values[i] = b;
  }

  /* Deactivate CS */
  cs_.high();
}

void At86rf215::blockAccessWrite(uint16_t address, uint8_t* values, uint16_t length)
{
  uint8_t address_hi, address_lo;

  /* Prepare device address */
  address_hi = (uint8_t) ((address & 0xFF00) >> 8);
  address_lo = (uint8_t) ((address & 0x00FF) >> 0);

  /* Prepare buffer for SPI transaction */
  values[0] = AT86RF215_WRITE_CMD | address_hi;
  values[1] = address_lo;

  /* Activate CS */
  cs_.low();

  for (uint16_t i = 0; i < length; i++)
  {
    /* Write byte to the SPI interface */
    spi_.rwByte(values[i]);
  }

  /* Deactivate CS */
  cs_.high();
}
