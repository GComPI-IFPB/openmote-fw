/**
 * @file       Serial.cpp
 * @author     Pere Tuset-Peiro (peretuset@openmote.com)
 * @version    v0.1
 * @date       May, 2015
 * @brief
 *
 * @copyright  Copyright 2015, OpenMote Technologies, S.L.
 *             This file is licensed under the GNU General Public License v2.
 */

/*================================ include ==================================*/

#include "Serial.hpp"
#include "BoardImplementation.hpp"

/*================================ define ===================================*/

/*================================ typedef ==================================*/

/*=============================== variables =================================*/

static const uint8_t CRC_LENGTH = 2;    // Length of the CRC

/*=============================== prototypes ================================*/

/*================================= public ==================================*/

Serial::Serial(Uart& uart):
  uart_(uart),
  receive_buffer_{0}, rxBuffer_(receive_buffer_, sizeof(receive_buffer_)),
  transmit_buffer_{0}, txBuffer_(transmit_buffer_, sizeof(transmit_buffer_)),
  rxCallback_(this, &Serial::rxCallback), txCallback_(this, &Serial::txCallback),
  useDma_(false), rxError_(false)
{
}

bool Serial::operator==(const Serial& other)
{
  return uart_ == other.uart_;
}

void Serial::init(void)
{
  /* Register UART callbacks */
  uart_.setRxCallback(&rxCallback_);
  uart_.setTxCallback(&txCallback_);
  
  /* Enable UART DMA */
  uart_.enableDMA();
  
  /* Enable UART interrupts */
  uart_.enableInterrupts();
}

void Serial::write(uint8_t* data, uint32_t size, bool useDma)
{
  Hdlc hdlc(txBuffer_);
  HdlcResult result = HdlcResult_Ok;
  uint8_t byte;
  bool status;
  
  /* Take the TX mutex */
  txMutex_.take();
  
  /* Store DMA usage */
  useDma_ = useDma;

  /* Reset the UART transmit buffer */
  txBuffer_.reset();

  /* Open the HDLC transmit buffer */
  result = hdlc.txOpen();
  if (result != HdlcResult_Ok) goto error;

  /* For each byte in the buffer */
  result = hdlc.txPut(data, size);
  if (result != HdlcResult_Ok) goto error;

  /* Close the HDLC buffer */
  result = hdlc.txClose();
  if (result != HdlcResult_Ok) goto error;
    
  /* Start transmission */
  if (!useDma)
  {
    /* Read first byte from the UART transmit buffer */
    status = txBuffer_.readByte(&byte);
    if (status != true) goto error;

    /* Write first byte to the UART */
    uart_.writeByte(byte);
  }
  else
  {
    /* Transmit all buffer at once */
    uart_.writeByte(txBuffer_.getHead(), txBuffer_.getSize());
  }
  
  /* Take the UART lock */
  uart_.txLock();
  
  /* Give the TX mutex */
  txMutex_.give();
    
  return;

error:
  /* Reset the UART buffer */
  txBuffer_.reset();

  /* Release the UART lock */
  uart_.txUnlock();

  return;
}

uint32_t Serial::read(uint8_t* buffer, uint32_t size)
{
  Buffer buffer_(buffer, size);
  Hdlc hdlc(buffer_);
  HdlcResult result;
  HdlcStatus status;
  uint32_t length;
  
  /* Restore receiver error */
  rxError_ = false;
   
  /* Lock the UART receive */
  uart_.rxLock();
    
  /* If there is an error */
  if (rxError_ == true)
  {
    return 0;
  }
  
  /* Open the HDLC receive buffer */
  hdlc.rxOpen();
      
  do
  {
    /* Parse the HDLC in the receive buffer */
    result = hdlc.rxPut(rxBuffer_);
    
    /* Get the HDLC status */
    status = hdlc.getRxStatus();
  } while(status != HdlcStatus_Done && result != HdlcResult_Error);

  /* If there is an error */
  if (result == HdlcResult_Error)
  {
    return 0;
  }
  
  /* Close the HDLC frame */
  result = hdlc.rxClose();
  
  /* If there is an error */
  if (result == HdlcResult_Error)
  {
    return 0;
  }
  
  /* Update the length value and account for the CRC bytes */
  length = buffer_.getSize() - CRC_LENGTH;

  /* Reset the receive buffer */
  rxBuffer_.reset();
  
  return length;
}

/*=============================== protected =================================*/

/*================================ private ==================================*/

void Serial::rxCallback(void)
{
  bool finished, status;
  
  /* Read bytes from UART */
  status = uart_.readBytes(rxBuffer_, finished);
  
  /* Check for errors */
  if (!status) goto error;
    
  /* If the UART has finished */
  if (finished)
  {
    /* Give the RX semaphore to signal end of reception */
    uart_.rxUnlockFromInterrupt();
  }
  
  return;
  
error:
  rxError_ = true;
  
  /* Give the RX semaphore to signal end of reception */
  uart_.rxUnlockFromInterrupt();
}

void Serial::txCallback(void)
{
  uint8_t byte;
  
  if (!useDma_)
  {
    /* Read byte from the UART transmit buffer */
    if (txBuffer_.readByte(&byte, true) == true)
    {
      /* Write byte to the UART */
      uart_.writeByte(byte);
    }
    else
    {
      /* Once done, free the UART lock */
      uart_.txUnlockFromInterrupt();
    }
  }
  else
  {
    /* Once done, free the UART lock */
    uart_.txUnlockFromInterrupt();
  }
}
