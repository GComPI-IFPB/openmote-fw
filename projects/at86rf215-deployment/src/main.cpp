/**
 * @file       main.cpp
 * @author     Pere Tuset-Peiro (peretuset@openmote.com)
 * @version    v0.1
 * @date       January, 2019
 * @brief
 *
 * @copyright  Copyright 2019, OpenMote Technologies, S.L.
 *             This file is licensed under the GNU General Public License v2.
 */

/*================================ include ==================================*/

#include "BoardImplementation.hpp"

#include "Gpio.hpp"
#include "Dma.hpp"

#include "Callback.hpp"
#include "Scheduler.hpp"
#include "Semaphore.hpp"
#include "Task.hpp"

#include "Serial.hpp"
#include "RadioBuffer.hpp"

#include "At86rf215.hpp"
#include "At86rf215_conf.h"

#include "Queue.hpp"

/*================================ define ===================================*/

#ifdef TRUE
#undef TRUE
#define TRUE                                ( 1 )
#endif

#ifdef FALSE
#undef FALSE
#define FALSE                               ( 0 )
#endif

#define GREEN_LED_TASK_PRIORITY             ( tskIDLE_PRIORITY + 0 )
#define RADIO_TASK_PRIORITY                 ( tskIDLE_PRIORITY + 1 )
#define SERIAL_TASK_PRIORITY                ( tskIDLE_PRIORITY + 2 )

#define UART_BAUDRATE						            ( 1267200 )
#define SPI_BAUDRATE                        ( 16000000 )

#define SERIAL_BUFFER_LENGTH                ( 512 )

#define USE_ENCRYPTION                      ( TRUE )
#define AES_KEY_LENGTH                      ( 16 )
#define AES_KEY_VALUES                      {0x0D, 0x0E, 0x0A, 0x0D, 0x0B, 0x0E, 0x0E, 0x0F, 0x0D, 0x0E, 0x0A, 0x0D, 0x0B, 0x0E, 0x0E, 0x0F}

#define QUEUE_LENGTH                        ( 8 )
#define QUEUE_READ_DELAY                    ( 10 )
#define QUEUE_WRITE_DELAY                   ( 10 )

#define RADIO_CORE_0                        ( At86rf215::CORE_RF09 )
#define RADIO_SETTINGS_0                    ( &radio_settings[CONFIG_FSK_OPTION1] )
#define RADIO_FREQUENCY_0                   ( &frequency_settings_09[FREQUENCY_09_FSK1] )
#define RADIO_CHANNEL_0                     ( 0 )
#define RADIO_CORE_1                        ( At86rf215::CORE_RF09 )
#define RADIO_SETTINGS_1                    ( &radio_settings[CONFIG_OQPSK_RATE4] )
#define RADIO_FREQUENCY_1                   ( &frequency_settings_09[FREQUENCY_09_OQPSK] )
#define RADIO_CHANNEL_1                     ( 0 )
#define RADIO_CORE_2                        ( At86rf215::CORE_RF09 )
#define RADIO_SETTINGS_2                    ( &radio_settings[CONFIG_OFDM2_MCS0] )
#define RADIO_FREQUENCY_2                   ( &frequency_settings_09[FREQUENCY_09_OFDM2] )
#define RADIO_CHANNEL_2                     ( 0 )

#define RADIO_CONFIG_ITEM                   ( 0 )
#define RADIO_CONFIG_ITEMS                  ( 3 )
#define RADIO_CORE                          ( radio_config[RADIO_CONFIG_ITEM].radio_core )
#define RADIO_SETTINGS                      ( radio_config[RADIO_CONFIG_ITEM].radio_settings )
#define RADIO_FREQUENCY                     ( radio_config[RADIO_CONFIG_ITEM].frequency_settings )
#define RADIO_CHANNEL                       ( radio_config[RADIO_CONFIG_ITEM].channel )
#define RADIO_TX_POWER                      ( At86rf215::TransmitPower::TX_POWER_MIN )

/*================================ typedef ==================================*/

typedef struct
{
  At86rf215::RadioCore radio_core;
  const radio_settings_t * radio_settings;
  const frequency_settings_t * frequency_settings;
  const uint16_t channel;
} radio_config_t;  

typedef struct
{
  RadioBuffer* rb;
  int8_t rssi;
} serial_message_t;

/*=============================== prototypes ================================*/

static void prvGreenLedTask(void *pvParameters);
static void prvRadioTask(void *pvParameters);
static void prvSerialTask(void *pvParameters);

static void radio_rx_init(void);
static void radio_rx_done(void);

static uint16_t prepare_serial(uint8_t* buffer_ptr, uint8_t* packet_ptr, uint16_t packet_length, int8_t rssi);

/*=============================== variables =================================*/

static RadioBufferManager rbm;
static Serial serial(uart0);

static Task heartbeatTask{(const char *) "Green", 128, GREEN_LED_TASK_PRIORITY, prvGreenLedTask, nullptr};
static Task radioTask{(const char *) "Radio", 128, RADIO_TASK_PRIORITY, prvRadioTask, nullptr};
static Task serialTask{(const char *) "Serial", 128, SERIAL_TASK_PRIORITY, prvSerialTask, nullptr};

static Queue<serial_message_t> queue(QUEUE_LENGTH);
static SemaphoreBinary radio_semaphore(false);

static PlainCallback radio_rx_init_cb(&radio_rx_init);
static PlainCallback radio_rx_done_cb(&radio_rx_done);

#if (USE_ENCRYPTION == TRUE)
static uint8_t aes_key[AES_KEY_LENGTH] = AES_KEY_VALUES;
#endif

static uint8_t serial_buffer[SERIAL_BUFFER_LENGTH];

static radio_config_t radio_config[RADIO_CONFIG_ITEMS] = {{RADIO_CORE_0, RADIO_SETTINGS_0, RADIO_FREQUENCY_0, RADIO_CHANNEL_0},
                                                          {RADIO_CORE_1, RADIO_SETTINGS_1, RADIO_FREQUENCY_1, RADIO_CHANNEL_1},
                                                          {RADIO_CORE_2, RADIO_SETTINGS_2, RADIO_FREQUENCY_2, RADIO_CHANNEL_2}};

/*================================= public ==================================*/

void main(void)
{
  /* Initialize the board */
  board.init();
    
  /* Initialize DMA controller */
  dma.init();
  
  /* Enable the SPI interface */
  spi0.enable(SPI_BAUDRATE);
  
  /* Enable the UART interface */
  uart0.enable(UART_BAUDRATE);
  
  /* Initialize Serial interface */
  serial.init();
  
  /* Initialize RadioBufferManager */
  rbm.init();
  
  /* Ensure 2.4 GHz antenna for CC2538 is used */
  antenna_at86rf215.low();
  antenna_cc2538.high();
  
  /* Start the scheduler */
  Scheduler::run();
}

/*================================ private ==================================*/

static void prvSerialTask(void *pvParameters)
{
  serial_message_t serial_message;
  bool received;

#if (USE_ENCRYPTION == TRUE)
  /* Enable AES */
  aes.enable();
  aes.loadKey(aes_key, AES_KEY_LENGTH);
#endif
  
  while(true)
  {
    /* Try to receive a message from the radio task */
    received = queue.receive(&serial_message, QUEUE_READ_DELAY);
    
    if (received)
    {
      uint8_t* buffer;
      uint16_t length;
      int8_t rssi;
      
      /* Turn on yellow LED */
      led_yellow.on();
      
      buffer = serial_message.rb->buffer;
      length = serial_message.rb->length;
      rssi = serial_message.rssi;
      
#if (USE_ENCRYPTION == TRUE)
      /* Decrypt payload */
      aes.decrypt(serial_message.rb->buffer, serial_message.rb->buffer, &serial_message.rb->length);
      
      /* True packet length is the first position */
      length = serial_message.rb->buffer[0];
#endif

      /* Prepare serial buffer */
      length = prepare_serial(serial_buffer, buffer, length, rssi);

      /* Send packet via Serial */
      serial.write(serial_buffer, length, true);
      
      /* Release RadioBuffer */
      rbm.release(serial_message.rb);

      /* Turn off yellow LED */
      led_yellow.off();
    }
  }
}

static void prvRadioTask(void *pvParameters)
{
	bool status; 
  
  do
  {
    /* Turn AT86RF215 radio on */
    at86rf215.on();
  
    /* Check AT86RF215 radio */
    status = at86rf215.check();
    
    /* If AT86RF215 radio not ready */
    if (!status)
    {
      /* Turn AT86RF215 radio off */
      at86rf215.off();
      
      /* Delay for 10 ms */
      Scheduler::delay_ms(10);
    }
  } while(!status);
  
  /* Set radio callbacks and enable interrupts */
  at86rf215.setRxCallbacks(RADIO_CORE, &radio_rx_init_cb, &radio_rx_done_cb);
  at86rf215.enableInterrupts();
  
  /* Wake up and configure radio */
  at86rf215.wakeup(RADIO_CORE);
  at86rf215.configure(RADIO_CORE, RADIO_SETTINGS, RADIO_FREQUENCY, RADIO_CHANNEL);
  at86rf215.setTransmitPower(RADIO_CORE, RADIO_TX_POWER);
  
  /* Try to receive a packet */
  at86rf215.receive(RADIO_CORE);
  
  /* Forever */
  do
  {
    At86rf215::RadioResult result;
    RadioBuffer* radio_buffer;
    bool status, received, crc;
    int8_t rssi, lqi;
    
    /* Get a RadioBuffer to work with */
    do {
      /* Try to get a RadioBuffer */
      status = rbm.get(&radio_buffer);
      if (!status)
      {
        /* Reset board */
        board.reset();
      }
    } while(!status);
    
    /* Wait until packet has been received */
    received = radio_semaphore.take();
    
    /* If we have received a packet */
    if (received == true)
    { 
      /* Set radio_buffer length to size */
      radio_buffer->length = radio_buffer->size;
      
      /* Recover packet from radio */
      result = at86rf215.getPacket(RADIO_CORE, radio_buffer->buffer, &radio_buffer->length, &rssi, &lqi, &crc);
    }
    
    /* Wake up and configure radio */
    at86rf215.wakeup(RADIO_CORE);
    at86rf215.configure(RADIO_CORE, RADIO_SETTINGS, RADIO_FREQUENCY, RADIO_CHANNEL);
    at86rf215.setTransmitPower(RADIO_CORE, RADIO_TX_POWER);
    
    /* Start the reception of a new packet */
    at86rf215.receive(RADIO_CORE);
    
    /* If we have received a packet, the radio is good and the CRC matches */
    if (received == true && result == At86rf215::RadioResult::Success && crc == true)
    {
      serial_message_t serial_message;
        
      /* Prepare serial message */
      serial_message.rb = radio_buffer;
      serial_message.rssi = lqi;
      
      /* Send message to queue */
      queue.send(&serial_message, QUEUE_WRITE_DELAY);
    }
    else
    {
      /* Otherwise, release RadioBuffer */
      rbm.release(radio_buffer);
    }
  } while(true);
  
  /* Turn AT86RF215 radio off */
  at86rf215.off();
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

static void radio_rx_init(void)
{
  /* Turn on orange LED */
  led_orange.on();
}

static void radio_rx_done(void)
{
  /* Turn off orange LED */
  led_orange.off();

  /* Notify we have transmitted a packet */
  radio_semaphore.giveFromInterrupt();
}

static uint16_t prepare_serial(uint8_t* buffer_ptr, uint8_t* packet_ptr, uint16_t packet_length, int8_t rssi)
{
  uint16_t length;

  /* Copy radio packet payload */
  dma.memcpy(buffer_ptr, packet_ptr, packet_length);

  /* Update buffer length */
  length = packet_length;

  /* Copy RSSI value */
  buffer_ptr[length++] = rssi;

  return length;
}
