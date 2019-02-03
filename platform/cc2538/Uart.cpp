/**
 * @file       platform_uart.cpp
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
#include "Uart.hpp"

#include "BoardImplementation.hpp"

#include "platform_includes.h"
#include "platform_types.h"

/*================================ define ===================================*/

#define UART_DATA                   (void *)(config_.base + UART_O_DR)

#define UART_RX_CHANNEL             ( UDMA_CH8_UART0RX | UDMA_PRI_SELECT )
#define UART_RX_CONTROL             ( UDMA_SIZE_8 | UDMA_SRC_INC_NONE | UDMA_DST_INC_8 | UDMA_ARB_1 )
#define UART_TX_CHANNEL             ( UDMA_CH9_UART0TX | UDMA_PRI_SELECT ) 
#define UART_TX_CONTROL             ( UDMA_SIZE_8 | UDMA_SRC_INC_8 | UDMA_DST_INC_NONE | UDMA_ARB_1 )

/*================================ typedef ==================================*/

/*=============================== variables =================================*/

/*=============================== prototypes ================================*/

/*================================= public ==================================*/

Uart::Uart(Gpio& rx, Gpio& tx, UartConfig& config):
  rx_(rx), tx_(tx), config_(config), \
  rxSemaphore_(false), txSemaphore_(false), \
  rx_callback_(nullptr), tx_callback_(nullptr)
{
}

bool Uart::operator==(const Uart& other) {
  return (config_ == other.config_);
}

void Uart::enable(uint32_t baudrate)
{
  /* Get GpioConfig structures */
  const GpioConfig& rx = rx_.getGpioConfig();
  const GpioConfig& tx = tx_.getGpioConfig();

  /* Store baudrate in configuration */
  if (baudrate != 0) {
    config_.baudrate = baudrate;
  }

  /* Enable peripheral except in deep sleep modes (e.g. LPM1, LPM2, LPM3) */
  SysCtrlPeripheralEnable(config_.peripheral);
  SysCtrlPeripheralSleepEnable(config_.peripheral);
  SysCtrlPeripheralDeepSleepDisable(config_.peripheral);

  /* Disable peripheral previous to configuring it */
  UARTDisable(config_.base);

  /* Set IO clock as UART clock source */
  UARTClockSourceSet(config_.base, config_.clock);

  /* Configure the UART RX and TX pins */
  IOCPinConfigPeriphInput(rx.port, rx.pin, rx.ioc);
  IOCPinConfigPeriphOutput(tx.port, tx.pin, tx.ioc);

  /* Configure the UART GPIOs */
  GPIOPinTypeUARTInput(rx.port, rx.pin);
  GPIOPinTypeUARTOutput(tx.port, tx.pin);

  /* Configure the UART */
  UARTConfigSetExpClk(config_.base, SysCtrlIOClockGet(), config_.baudrate, config_.mode);

  /* Disable FIFO as we only use a one-byte buffer */
  UARTFIFODisable(config_.base);

  /* Raise an interrupt at the end of transmission */
  UARTTxIntModeSet(config_.base, UART_TXINT_MODE_EOT);

  /* Enable UART hardware */
  UARTEnable(config_.base);
}

void Uart::sleep(void)
{
  /* Get GpioConfig structures */
  const GpioConfig& rx = rx_.getGpioConfig();
  const GpioConfig& tx = tx_.getGpioConfig();

  /* Wait until UART is not busy */
  while(UARTBusy(config_.base))
    ;

  /* Disable UART hardware */
  UARTDisable(config_.base);

  /* Configure the pins as outputs */
  GPIOPinTypeGPIOInput(rx.port, rx.pin);
  GPIOPinTypeGPIOInput(tx.port, tx.pin);
}

void Uart::wakeup(void)
{
  /* Re-enable the UART interface */
  enable();
}

void Uart::setRxCallback(Callback* callback)
{
  rx_callback_ = callback;
}

void Uart::setTxCallback(Callback* callback)
{
  tx_callback_ = callback;
}

void Uart::enableInterrupts(void)
{
  /* Register the interrupt handler */
  InterruptHandler::getInstance().setInterruptHandler(*this);

  /* Clear the UART RX, TX and RX timeout interrupts */
  UARTIntClear(config_.base, UART_INT_RX | UART_INT_TX);
  
  /* Enable the UART RX, TX and RX timeout interrupts */
  UARTIntEnable(config_.base, UART_INT_RX | UART_INT_TX);

  /* Set the UART interrupt priority */
  IntPrioritySet(config_.interrupt, 0xF0);

  /* Enable the UART interrupt */
  IntEnable(config_.interrupt);
}

void Uart::disableInterrupts(void)
{
  /* Disable the UART RX, TX and RX timeout interrupts */
  UARTIntDisable(config_.base, UART_INT_RX | UART_INT_TX);

  /* Disable the UART interrupt */
  IntDisable(config_.interrupt);
}

void Uart::rxLock(void)
{
  rxSemaphore_.take();
}

void Uart::txLock(void)
{
  txSemaphore_.take();
}

void Uart::rxUnlock(void)
{
  rxSemaphore_.give();
}

void Uart::txUnlock(void)
{
  txSemaphore_.give();
}

void Uart::rxUnlockFromInterrupt(void)
{
  rxSemaphore_.giveFromInterrupt();
}

void Uart::txUnlockFromInterrupt(void)
{
  txSemaphore_.giveFromInterrupt();
}

uint8_t Uart::readByte(void)
{
  int32_t byte;
  byte = UARTCharGetNonBlocking(config_.base);
  return (uint8_t)(byte & 0xFF);
}

uint32_t Uart::readByte(uint8_t * buffer, uint32_t length)
{
  uint32_t data;

  /* Read all bytes from UART (blocking) */
  for (uint32_t i = 0; i < length; i++)
  {
    data = UARTCharGet(config_.base);
    *buffer++ = (uint8_t)data;
  }

  /* Wait until it is complete */
  while(UARTBusy(config_.base))
    ;

  return 0;
}

void Uart::writeByte(uint8_t byte)
{
  UARTCharPutNonBlocking(config_.base, byte);
}

int32_t Uart::writeByte(uint8_t * buffer, uint32_t length)
{
  /* Put all bytes to UART (blocking) */
  for (uint32_t i = 0; i < length; i++)
  {
    UARTCharPut(config_.base, *buffer++);
  }

  return 0;
}

int32_t Uart::writeByteDma(uint8_t * buffer, uint32_t length)
{
  /* Enable uDMA complete interrupt for UART TX */
  UARTDMAEnable(config_.base, UART_DMA_TX);
  
  /* Setup DMA control for UART transmission */
  uDMAChannelControlSet(UART_TX_CHANNEL, UART_TX_CONTROL);
  
  /* Setup DMA buffers for UART using BASIC mode */
  uDMAChannelTransferSet(UART_TX_CHANNEL, UDMA_MODE_BASIC, buffer, UART_DATA, length);

  /* Enable the TX channel */
  uDMAChannelEnable(UART_TX_CHANNEL);
  
  /* Busy-wait until there are no more bytes to be tramsmitted */
  // while(uDMAChannelSizeGet(UART_TX_CHANNEL) > 0);
  
  /* Disable uDMA complete interrupt for UART RX and TX */
  // UARTDMADisable(config_.base, UART_DMA_TX);
  
  return 0;
}

/*=============================== protected =================================*/

UartConfig& Uart::getConfig(void)
{
  return config_;
}

void Uart::interruptHandler(void)
{
  uint32_t status;

  /* Read interrupt source */
  status = UARTIntStatus(config_.base, true);

  /* Clear UART interrupt in the NVIC */
  IntPendClear(config_.interrupt);

  /* Process TX interrupt */
  if (status & UART_INT_TX)
  {
    UARTIntClear(config_.base, UART_INT_TX);
    interruptHandlerTx();
  }

  /* Process RX interrupt */
  if ((status & UART_INT_RX))
  {
    UARTIntClear(config_.base, UART_INT_RX);
    interruptHandlerRx();
  }
}

/*================================ private ==================================*/

void Uart::interruptHandlerRx(void)
{
  if (rx_callback_ != nullptr)
  {
    rx_callback_->execute();
  }
}

void Uart::interruptHandlerTx(void)
{
  if (tx_callback_ != nullptr)
  {
    tx_callback_->execute();
  }
}
