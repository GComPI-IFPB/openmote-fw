/**
 * @file       main.cpp
 * @author     Pere Tuset-Peiro (peretuset@openmote.com)
 * @version    v0.1
 * @date       February, 2019
 * @brief
 *
 * @copyright  Copyright 2019, OpenMote Technologies, S.L.
 *             This file is licensed under the GNU General Public License v2.
 */

/*================================ include ==================================*/

#include "BoardImplementation.hpp"

#include "Gpio.hpp"

#include "Callback.hpp"
#include "Scheduler.hpp"
#include "Semaphore.hpp"
#include "Task.hpp"

#include "Serial.hpp"

#include "At86rf215.hpp"
#include "At86rf215_conf.h"

/*================================ define ===================================*/

#define GREEN_LED_TASK_PRIORITY             ( tskIDLE_PRIORITY + 0 )
#define RADIO_TASK_PRIORITY                 ( tskIDLE_PRIORITY + 1 )
#define SERIAL_TASK_PRIORITY                ( tskIDLE_PRIORITY + 2 )

#define UART_BAUDRATE                       ( 1843200 )
#define SPI_BAUDRATE                        ( 16000000 )

#define RADIO_BUFFER_LENGTH                 ( 2048 )
#define SERIAL_BUFFER_LENGTH                ( 2048 )

#define RADIO_CORE                          ( At86rf215::CORE_RF09 )
#define RADIO_SETTINGS                      ( &radio_settings[CONFIG_OFDM1_MCS6] )
#define RADIO_FREQUENCY                     ( &frequency_settings[FREQUENCY_OFDM1] )
#define RADIO_TX_POWER                      ( At86rf215::TransmitPower::TX_POWER_10 )

/*================================ typedef ==================================*/

enum RadioMode {
  RadioMode_Transmit = 0x30,
  RadioMode_Receive  = 0x31
};

/*=============================== prototypes ================================*/

static void prvGreenLedTask(void *pvParameters);
static void prvRadioTask(void *pvParameters);
static void prvSerialTask(void *pvParameters);

static void radio_rx_init(void);
static void radio_rx_done(void);
static void radio_tx_init(void);
static void radio_tx_done(void);

static bool radio_rx_packet(void);
static bool radio_tx_packet(void);

/*=============================== variables =================================*/

static Serial serial(uart0);

static Task heartbeatTask{(const char *) "Green", 128, GREEN_LED_TASK_PRIORITY, prvGreenLedTask, nullptr};
static Task receiveTask{(const char *) "Radio", 128, RADIO_TASK_PRIORITY, prvRadioTask, nullptr};
static Task serialTask{(const char *) "Serial", 128, SERIAL_TASK_PRIORITY, prvSerialTask, nullptr};

static PlainCallback radio_rx_init_cb(&radio_rx_init);
static PlainCallback radio_rx_done_cb(&radio_rx_done);
static PlainCallback radio_tx_init_cb(&radio_tx_init);
static PlainCallback radio_tx_done_cb(&radio_tx_done);

static SemaphoreBinary rx_semaphore(false);
static SemaphoreBinary tx_semaphore(false);

static SemaphoreBinary radio_start(false);
static SemaphoreBinary radio_done(false);

static uint8_t serial_buffer[SERIAL_BUFFER_LENGTH];
static uint16_t serial_buffer_len = sizeof(serial_buffer);

static uint8_t radio_buffer[RADIO_BUFFER_LENGTH];
static uint16_t radio_buffer_len = sizeof(radio_buffer);

static RadioMode radioMode;

/*================================= public ==================================*/

int main(void)
{
  /* Initialize the board */
  board.init();
  
  /* Enable the SPI interface */
  spi0.enable(SPI_BAUDRATE);
  
  /* Enable the UART interface */
  uart0.enable(UART_BAUDRATE);
  
  /* Initialize Serial interface */
  serial.init();

  /* Start the scheduler */
  Scheduler::run();
}

/*================================ private ==================================*/

static void prvSerialTask(void *pvParameters)
{
  bool status; 

  /* Turn AT86RF215 radio on */
  at86rf215.on();
  
  /* Check AT86RF215 radio */
  status = at86rf215.check();
  if (!status)
  {
    /* Blink red LED */ 
    board.error();
  }
  
  /* Set radio callbacks and enable interrupts */
  at86rf215.setTxCallbacks(RADIO_CORE, &radio_tx_init_cb, &radio_tx_done_cb);
  at86rf215.setRxCallbacks(RADIO_CORE, &radio_rx_init_cb, &radio_rx_done_cb);
  at86rf215.enableInterrupts();
  
  /* Wake up and configure radio */
  at86rf215.wakeup(RADIO_CORE);
  at86rf215.configure(RADIO_CORE, RADIO_SETTINGS, RADIO_FREQUENCY);
  at86rf215.setTransmitPower(RADIO_CORE, RADIO_TX_POWER);

  /* Ready to transmit */
  at86rf215.ready(RADIO_CORE);
  
  /* Forever */
  while (true)
  {
    /* Restore serial_rx pointer and length */
    uint8_t* serial_ptr = serial_buffer;
    uint16_t serial_len = sizeof(serial_buffer);
        
    /* Block until we get a message from the serial */
    serial_len = serial.read(serial_ptr, serial_len);
    
    /* If this is a setup message */
    if (serial_len == 1)
    {
      /* Set mode according to serial */
      radioMode = (RadioMode) serial_ptr[0];
    
      /* Signal start */
      radio_start.give();
    
      /* Block until finished */
      radio_done.take();
    }
  }
  
  /* Turn AT86RF215 radio off */
  at86rf215.off();
}

static void prvRadioTask(void *pvParameters)
{
  /* Forever */
  while (true)
  {
    bool finished = false;
    
    /* Wait for start signal */
    radio_start.take();
    
    /* In transmit mode we start by transmitting a packet */
    if (radioMode == RadioMode_Transmit)
    {
      bool result;
      
      /* Wait for a packet from serial and transmit it to the radio */
      result = radio_tx_packet();
      
      /* If error, skip loop */
      if (!result)
      {
        finished = true;
      }
    }
    
    /* Run until finish condition */
    while (!finished)
    {
      bool result;
      
      /* Start receiving and transmit the packet through the serial */
      result = radio_rx_packet();
      
      /* If successul transmit, otherwise break the loop */
      if (result)
      {
        /* Wait for a packet from serial and transmit it to the radio */
        result = radio_tx_packet();
        
        /* If not successful, break the loop */
        if (!result)
        {
          finished = true;
        }
      }
      else
      {
        finished = true;
      }
    }
    
    /* Signal stop */
    radio_done.give();
  }
}

static bool radio_rx_packet(void)
{
  At86rf215::RadioResult result;
  bool taken;
  bool success;

  /* Restore serial buffer pointer and length */
  uint8_t* radio_ptr = radio_buffer;
  uint16_t radio_len = radio_buffer_len;
  
  /* Turn on orange LED */
  led_orange.on();
  
  /* Receive packet */
  at86rf215.receive(RADIO_CORE);
        
  /* Wait until packet has been received or 2 seconds */
  taken = rx_semaphore.take(2000);
  
  /* If the semaphore could be taken */
  if (taken)
  {
    int8_t rssi, lqi;
    bool crc;
    
    /* Load packet to radio */
    result = at86rf215.getPacket(RADIO_CORE, radio_ptr, &radio_len, &rssi, &lqi, &crc);
    
    /* Go back to ready */
    at86rf215.ready(RADIO_CORE);
    
    /* Check packet has been received successfully */
    if (result == At86rf215::Success && crc == true)
    {
      /* Write radio buffer via Serial */
      serial.write(radio_ptr, radio_len, true);
      
      /* Mark result as success */
      success = true;
    }
    else
    {
      /* Mark result as not success */
      success = false;
    }
  }
  else
  {
    /* Turn off yellow LED */
    led_yellow.off();
    
    /* Mark result as not success */
    success = false;
  }
  
  /* Turn off orange LED */
  led_orange.off();
  
  return success;
}

static bool radio_tx_packet(void)
{
  bool success;
  
  /* Restore serial buffer pointer and length */
  uint8_t* serial_ptr = serial_buffer;
  uint16_t serial_len = sizeof(serial_buffer);
  
  /* Block until we get a message from the serial */
  serial_len = serial.read(serial_ptr, serial_len);
  
  /* If we have received a serial message */
  if (serial_len > 0)
  {
    uint32_t length;
    bool taken;
    
    /* Copy packet to radio transmit buffer */
    length = dma.memcpy(radio_buffer, serial_ptr, serial_len);

    /* Turn on orange LED */
    led_orange.on();
    
    /* Load packet to radio */
    at86rf215.loadPacket(RADIO_CORE, radio_buffer, length);
    
    /* Transmit packet */
    at86rf215.transmit(RADIO_CORE);
    
    /* Wait until packet has been transmitted */
    taken = tx_semaphore.take(2000);
    
    if (taken)
    {
      /* Mark result as success */
      success = true;
    }
    else
    {
      /* Turn off yellow LED */
      led_yellow.off();
    
      /* Mark result as not success */
      success = false;
    }
    
    /* Go back to ready */
    at86rf215.ready(RADIO_CORE);
    
    /* Turn off orange LED */
    led_orange.off();
  }
  else
  {
    /* Mark result as not success */
    success = false;
  }
  
  return success;
}

static void prvGreenLedTask(void *pvParameters)
{
  /* Forever */
  while (true)
  {
    /* Turn on green LED for 100 ms */
    led_green.on();
    Scheduler::delay_ms(100);
    
    /* Turn off green LED for 900 ms */
    led_green.off();
    Scheduler::delay_ms(900);
  }
}

static void radio_tx_init(void)
{
  /* Turn on yellow LED */
  led_yellow.on();
}

static void radio_tx_done(void)
{
  /* Turn off yellow LED */
  led_yellow.off();

  /* Notify we have transmitted a packet */
  tx_semaphore.giveFromInterrupt();
}

static void radio_rx_init(void)
{
  /* Turn on yellow LED */
  led_yellow.on();
}

static void radio_rx_done(void)
{
  /* Turn off yellow LED */
  led_yellow.off();
  
  /* Notify we have received a packet */
  rx_semaphore.giveFromInterrupt();
}