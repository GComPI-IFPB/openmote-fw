/**
 * @file       Spi.cpp
 * @author     Pere Tuset-Peiro (peretuset@openmote.com)
 * @version    v0.1
 * @date       November, 2018
 * @brief
 *
 * @copyright  Copyright 2018, OpenMote Technologies, S.L.
 *             This file is licensed under the GNU General Public License v2.
 */

/*================================ include ==================================*/

#include "Gpio.hpp"
#include "InterruptHandler.hpp"
#include "Spi.hpp"

#include "platform_includes.h"
#include "platform_types.h"

/*================================ define ===================================*/

#define SSI_PRIO                    ( 0xF0 )

#define SSI_DATA                    (void *)(config_.base + SSI_O_DR)

#define UDMA_CH10_SSI0RX_MASK       (1 << UDMA_CH10_SSI0RX)
#define UDMA_CH11_SSI0TX_MASK       (1 << UDMA_CH11_SSI0TX)

/*================================ typedef ==================================*/

/*=============================== variables =================================*/

#if defined(__IAR_SYSTEMS_ICC__)
#pragma data_alignment=1024
unsigned char ucDMAControlTable[1024];
#else
unsigned char ucDMAControlTable[1024] __attribute__ ((aligned(1024)));
#endif

/*=============================== prototypes ================================*/

/*================================= public ==================================*/

Spi::Spi(Gpio& miso, Gpio& mosi, Gpio& clk, SpiConfig& config):
  miso_(miso), mosi_(mosi), clk_(clk), config_(config),
  rx_callback_(nullptr), tx_callback_(nullptr)
{
}

void Spi::enable(uint32_t baudrate)
{
  /* Get MISO, MOSI and CLK config */
  GpioConfig& miso = miso_.getGpioConfig();
  GpioConfig& mosi = mosi_.getGpioConfig();
  GpioConfig& clk  = clk_.getGpioConfig();

  /* Store baudrate in configuration */
  if (baudrate != 0) {
    config_.baudrate = baudrate;
  }
  
  /* Enable peripheral except in deep sleep modes (e.g. LPM1, LPM2, LPM3) */
  SysCtrlPeripheralEnable(config_.peripheral);
  SysCtrlPeripheralSleepEnable(config_.peripheral);
  SysCtrlPeripheralDeepSleepDisable(config_.peripheral);

  /* Reset peripheral previous to configuring it */
  SSIDisable(config_.base);

  /* Set IO clock as SPI clock source */
  SSIClockSourceSet(config_.base, config_.clock);

  /* Configure the MISO, MOSI and CLK pins as peripheral */
  IOCPinConfigPeriphInput(miso.port, miso.pin, miso.ioc);
  IOCPinConfigPeriphOutput(mosi.port, mosi.pin, mosi.ioc);
  IOCPinConfigPeriphOutput(clk.port, clk.pin, clk.ioc);
  
  /* Configure MISO, MOSI and CLK GPIOs */
  GPIOPinTypeSSI(miso.port, miso.pin);
  GPIOPinTypeSSI(mosi.port, mosi.pin);
  GPIOPinTypeSSI(clk.port, clk.pin);
  
  /* Configure the SPI clock according to its input */
  if (config_.clock == SSI_CLOCK_SYSTEM)
  {
    uint32_t clock = SysCtrlClockGet();
    SSIConfigSetExpClk(config_.base, clock, config_.protocol, \
                       config_.mode, config_.baudrate, config_.datawidth);
  }
  else if (config_.clock == SSI_CLOCK_PIOSC)
  {
    uint32_t clock = SysCtrlIOClockGet();
    SSIConfigSetExpClk(config_.base, clock, config_.protocol, \
                       config_.mode, config_.baudrate, config_.datawidth);
  }
  else
  {
    while(true)
      ;
  }

  /* Enable the SPI module */
  SSIEnable(config_.base);
}

void Spi::sleep(void)
{
  /* Get MISO, MOSI and CLK config */
  GpioConfig& miso = miso_.getGpioConfig();
  GpioConfig& mosi = mosi_.getGpioConfig();
  GpioConfig& clk  = clk_.getGpioConfig();

  /* Disable the SPI module */
  SSIDisable(config_.base);

  /* Configure the MISO, MOSI and CLK pins as output */
  GPIOPinTypeGPIOInput(miso.port, miso.pin);
  GPIOPinTypeGPIOInput(mosi.port, mosi.pin);
  GPIOPinTypeGPIOInput(clk.port, clk.pin);
}

void Spi::wakeup(void)
{
  enable();
}

void Spi::setRxCallback(Callback* callback)
{
  rx_callback_ = callback;
}

void Spi::setTxCallback(Callback* callback)
{
  tx_callback_ = callback;
}

void Spi::initDma(void)
{
  /* Enable the uDMA controller */
  uDMAEnable();

  /* Set the base for the channel control table */
  uDMAControlBaseSet(&ucDMAControlTable[0]);
}

void Spi::enableDma(void)
{
  /* Enable uDMA complete interrupt for SPI RX and TX */
  SSIDMAEnable(config_.base, SSI_DMA_RX | SSI_DMA_TX);
  
  /* No attributes need to be set for a perpheral-based transfer.
   * The attributes are cleared by default, but are explicitly cleared
   * here, in case they were set elsewhere.
   */
  // uDMAChannelAttributeDisable(UDMA_CH10_SSI0RX, UDMA_ATTR_ALL);
  // uDMAChannelAttributeDisable(UDMA_CH11_SSI0TX, UDMA_ATTR_ALL);
  
  // uDMAChannelAttributeEnable(UDMA_CH10_SSI0RX, UDMA_ATTR_HIGH_PRIORITY);
  // uDMAChannelAttributeEnable(UDMA_CH11_SSI0TX, UDMA_ATTR_HIGH_PRIORITY);
}

void Spi::disableDma(void)
{
  /* Disable uDMA complete interrupt for SPI RX and TX */
  SSIDMADisable(config_.base, SSI_DMA_RX | SSI_DMA_TX);
}

void Spi::enableInterrupts(void)
{
  /* Register the interrupt handler */
  InterruptHandler::getInstance().setInterruptHandler(this);

  /* Enable the SPI interrupt */
  SSIIntEnable(config_.base, (SSI_TXFF | SSI_RXFF | SSI_RXTO | SSI_RXOR));

  /* Set the SPI interrupt priority */
  IntPrioritySet(config_.interrupt, SSI_PRIO);
  
  /* Enable the SPI interrupt */
  IntEnable(config_.interrupt);
}

void Spi::disableInterrupts(void)
{
  /* Disable the SPI interrupt */
  SSIIntDisable(config_.base, (SSI_TXFF | SSI_RXFF | SSI_RXTO | SSI_RXOR));

  /* Disable the SPI interrupt */
  IntDisable(config_.interrupt);
}

uint8_t Spi::rwByte(uint8_t byte)
{
  uint32_t ret;

  /* Push a byte */
  SSIDataPut(config_.base, byte);

  /* Wait until it is complete */
  while(SSIBusy(config_.base))
    ;

  /* Read a byte */
  SSIDataGet(config_.base, &ret);

  return (uint8_t)(ret & 0xFF);
}

bool Spi::rwByte(uint8_t* transmitBuffer, uint32_t transmitLength, uint8_t* receiveBuffer, uint32_t receiveLength)
{
  /* Check transmit and receive buffer size */
  if ((transmitLength == 0) || (receiveLength == 0) || (transmitLength != receiveLength))
  {
    return false;
  }

  /* Iterate over all positions in transmitBuffer */
  for (uint32_t i =  0; i < transmitLength; i++)
  {
    uint32_t data;
    uint8_t byte;
    
    /* Get next byte */
    byte = transmitBuffer[i];
    
    /* Transmit next byte */
    SSIDataPut(config_.base, byte);

    /* Wait until it is complete */
    while(SSIBusy(config_.base))
        ;

    /* Read a byte */
    SSIDataGet(config_.base, &data);

    /* Store the result */
    receiveBuffer[i] = (uint8_t)(data & 0xFF);
  }

  return true;
}

bool Spi::rwByteDma(uint8_t* transmitBuffer, uint32_t transmitLength, uint8_t* receiveBuffer, uint32_t receiveLength)
{
  /* Check transmit and receive buffer size */
  if ((transmitLength == 0) || (receiveLength == 0) || (transmitLength != receiveLength))
  {
    return false;
  }
    
  /* Setup DMA control for SPI transmission and reception */
  uDMAChannelControlSet(UDMA_CH10_SSI0RX | UDMA_PRI_SELECT, 
                        UDMA_SIZE_8 | UDMA_SRC_INC_NONE | UDMA_DST_INC_8 | UDMA_ARB_1);
  uDMAChannelControlSet(UDMA_CH11_SSI0TX | UDMA_PRI_SELECT, 
                        UDMA_SIZE_8 | UDMA_SRC_INC_8 | UDMA_DST_INC_NONE | UDMA_ARB_1);
  
  /* Setup DMA buffers for SPI using BASIC mode */
  uDMAChannelTransferSet(UDMA_CH10_SSI0RX | UDMA_PRI_SELECT,
                         UDMA_MODE_BASIC, SSI_DATA, receiveBuffer, receiveLength);
  uDMAChannelTransferSet(UDMA_CH11_SSI0TX | UDMA_PRI_SELECT,
                         UDMA_MODE_BASIC, transmitBuffer, SSI_DATA, transmitLength);

  /* Enable the TX channel */
  uDMAChannelEnable(UDMA_CH10_SSI0RX);
  uDMAChannelEnable(UDMA_CH11_SSI0TX);
  
  /* Busy-wait until there are no more bytes to be received */
  while(uDMAChannelSizeGet(UDMA_CH10_SSI0RX) > 0);
  
  return true;
}

/*=============================== protected =================================*/

SpiConfig& Spi::getConfig(void)
{
    return config_;
}

void Spi::interruptHandler(void)
{
  uint32_t status;

  /* Read interrupt source */
  status = SSIIntStatus(config_.base, true);

  /* Clear SPI interrupt in the NVIC */
  IntPendClear(config_.interrupt);

  /* Process TX interrupt */
  if (status & SSI_TXFF) {
    interruptHandlerTx();
  }

  /* Process RX interrupt */
  if (status & SSI_RXFF) {
    interruptHandlerRx();
  }
}

/*================================ private ==================================*/

void Spi::interruptHandlerRx(void)
{
  if (tx_callback_ != nullptr)
  {
    tx_callback_->execute();
  }
}

void Spi::interruptHandlerTx(void)
{
  if (rx_callback_ != nullptr)
  {
    rx_callback_->execute();
  }
}
