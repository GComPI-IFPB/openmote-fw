/**
 * @file       Uart.cpp
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
  rx_callback_(nullptr), tx_callback_(nullptr),
  rx_timeout_(false)
{
}

bool Uart::operator==(const Uart& other)
{
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
  
  /* Configure the UART clock according to its input */
  if (config_.clock == UART_CLOCK_SYSTEM)
  {
    uint32_t clock = SysCtrlClockGet();
    UARTConfigSetExpClk(config_.base, clock, config_.baudrate, config_.mode);
  }
  else if (config_.clock == UART_CLOCK_PIOSC)
  {
    uint32_t clock = SysCtrlIOClockGet();
    UARTConfigSetExpClk(config_.base, clock, config_.baudrate, config_.mode);
  }
  else
  {
    while(true)
      ;
  }

  /* Enable FIFO operation to reduce interrupts */
  UARTFIFOLevelSet(config_.base, UART_FIFO_TX1_8, UART_FIFO_RX7_8);
  UARTFIFOEnable(config_.base);

  /* Raise an interrupt at the end of transmission */
  // UARTTxIntModeSet(config_.base, UART_TXINT_MODE_FIFO | UART_TXINT_MODE_EOT);
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
  UARTIntClear(config_.base, UART_INT_RX | UART_INT_TX | UART_INT_RT);
  
  /* Enable the UART RX, TX and RX timeout interrupts */
  UARTIntEnable(config_.base, UART_INT_RX | UART_INT_TX | UART_INT_RT);

  /* Set the UART interrupt priority */
  IntPrioritySet(config_.interrupt, 0xF0);

  /* Enable the UART interrupt */
  IntEnable(config_.interrupt);
}

void Uart::disableInterrupts(void)
{
  /* Disable the UART RX, TX and RX timeout interrupts */
  UARTIntDisable(config_.base, UART_INT_RX | UART_INT_TX | UART_INT_RT);

  /* Disable the UART interrupt */
  IntDisable(config_.interrupt);
}

void Uart::enableDMA(void)
{
  /* Enable uDMA complete interrupt for UART TX */
  UARTDMAEnable(config_.base, UART_DMA_TX);
}

void Uart::disableDMA(void)
{
  /* Disable uDMA complete interrupt for UART RX and TX */
  UARTDMADisable(config_.base, UART_DMA_TX);
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

bool Uart::readAvailable(void)
{
  return UARTCharsAvail(config_.base);
}

bool Uart::readTimeout(void)
{
  return rx_timeout_;
}

bool Uart::readByte(uint8_t* byte)
{
  bool status = false;
  
  /* Check if there are bytes available */
  if (UARTCharsAvail(config_.base) == true)
  {
    int32_t scratch;
    
    /* Read byte from UART, non-blocking */
    scratch = UARTCharGetNonBlocking(config_.base);
    if (scratch >= 0)
    {
      *byte = (uint8_t) scratch;
      status = true;
    }
  }
  
  return status;
}

uint32_t Uart::readByte(uint8_t* buffer, uint32_t length)
{
  uint32_t count = 0;
  
  /* Check if there are bytes available */
  while (UARTCharsAvail(config_.base) && length > 0)
  {
    int32_t scratch;
        
    scratch = UARTCharGetNonBlocking(config_.base);
    if (scratch >= 0)
    {
      /* Read byte from UART, blocking */
      *buffer++ = (uint8_t) scratch;
      count++;
      length--;
    }
  }

  return count;
}

void Uart::writeByte(uint8_t byte)
{
  /* Write byte to UART, non-blocking */
  UARTCharPutNonBlocking(config_.base, byte);
}

uint32_t Uart::writeByte(uint8_t* buffer, uint32_t length)
{  
  /* Setup DMA control for UART transmission */
  uDMAChannelControlSet(UART_TX_CHANNEL, UART_TX_CONTROL);
  
  /* Setup DMA buffers for UART using BASIC mode */
  uDMAChannelTransferSet(UART_TX_CHANNEL, UDMA_MODE_BASIC, buffer, UART_DATA, length);

  /* Enable the TX channel */
  uDMAChannelEnable(UART_TX_CHANNEL);
  
  /* Busy-wait until there are no more bytes to be tramsmitted */
  // while(uDMAChannelSizeGet(UART_TX_CHANNEL) > 0);
  
  return 0;
}

uint32_t Uart::readBytes(uint8_t* buffer, uint32_t length)
{
  uint32_t data;

  /* Read all bytes from UART (blocking) */
  for (uint32_t i = 0; i < length; i++)
  {
    /* Read byte from UART, blocking */
    data = UARTCharGet(config_.base);
    *buffer++ = (uint8_t)data;
  }

  /* Wait until it is complete */
  while(UARTBusy(config_.base))
    ;

  return 0;
}
 uint32_t Uart::writeBytes(uint8_t* buffer, uint32_t length)
{
  /* Write all bytes to UART */
  for (uint32_t i = 0; i < length; i++)
  {
    /* Write byte to UART, blocking */
    UARTCharPut(config_.base, *buffer++);
  }

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

  /* Read interrupt status */
  status = UARTIntStatus(config_.base, true);

  /* Clear UART interrupts */
  UARTIntClear(config_.base, status);
  
  /* Process TX interrupt */
  if (status & UART_INT_TX)
  {
    interruptHandlerTx();
  }
  
  /* Process RX interrupt */
  if (status & (UART_INT_RX | UART_INT_RT))
  {
    rx_timeout_ = ((status & UART_INT_RT) == UART_INT_RT);
    interruptHandlerRx();
  }
}

/*================================ private ==================================*/

inline void Uart::interruptHandlerRx(void)
{
  if (rx_callback_ != nullptr)
  {
    rx_callback_->execute();
  }
}

inline void Uart::interruptHandlerTx(void)
{
  if (tx_callback_ != nullptr)
  {
    tx_callback_->execute();
  }
}
