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

#include "Callback.hpp"
#include "Scheduler.hpp"
#include "Semaphore.hpp"
#include "Task.hpp"

#include "Serial.hpp"

#include "At86rf215.hpp"
#include "At86rf215_conf.h"

/*================================ define ===================================*/

#define GREEN_LED_TASK_PRIORITY (tskIDLE_PRIORITY + 0)
#define RADIO_TASK_PRIORITY (tskIDLE_PRIORITY + 1)

#define UART_BAUDRATE (115200)
#define SPI_BAUDRATE (8000000)

#define RADIO_BUFFER_LENGTH (1024)
#define SERIAL_BUFFER_LENGTH (1024)

#define RADIO_CORE (At86rf215::CORE_RF09)

/*  OFDM Radio settings */
// #define RADIO_SETTINGS (&radio_settings[CONFIG_OFDM2_MCS0])          /* BPSK,   rate 1/2, 4x repetition,   50 kbps */
// #define RADIO_FREQUENCY (&frequency_settings_09[FREQUENCY_09_OFDM2]) /* OFDM Mode 2,  800 kHz */

/* FSK Radio settings */
#define RADIO_SETTINGS (&radio_settings[CONFIG_FSK_OPTION1])        /* X 2-FSK,  50 kbps */
#define RADIO_FREQUENCY (&frequency_settings_09[FREQUENCY_09_FSK1]) /* FSK Mode 1,   200 kHz */

/* OQPSK Radio settings */
// #define RADIO_SETTINGS (&radio_settings[CONFIG_OQPSK_RATE4])         /* X OQPSK-DSSS,  100 kchips/s,  50.00 kbps */
// #define RADIO_FREQUENCY (&frequency_settings_09[FREQUENCY_09_OQPSK]) /* OQPSK,        600 kHz */

#define RADIO_CHANNEL (0)
#define RADIO_TX_POWER (At86rf215::TransmitPower::TX_POWER_MIN)

/*================================ typedef ==================================*/

/*=============================== prototypes ================================*/

static void prvGreenLedTask(void *pvParameters);
static void prvRadioTask(void *pvParameters);

static void radio_rx_init(void);
static void radio_rx_done(void);

static uint16_t prepare_serial(uint8_t *buffer_ptr, uint8_t *packet_ptr, uint16_t packet_length, int8_t lqi);

/*=============================== variables =================================*/

static Serial serial(uart0);

static Task heartbeatTask{(const char *)"Green", 128, (unsigned char)GREEN_LED_TASK_PRIORITY, prvGreenLedTask, nullptr};
static Task radioTask{(const char *)"Radio", 128, (unsigned char)RADIO_TASK_PRIORITY, prvRadioTask, nullptr};

static PlainCallback radio_rx_init_cb(&radio_rx_init);
static PlainCallback radio_rx_done_cb(&radio_rx_done);

static SemaphoreBinary semaphore(false);

static uint8_t serial_buffer[SERIAL_BUFFER_LENGTH];

static uint8_t radio_buffer[RADIO_BUFFER_LENGTH];
static uint16_t radio_buffer_len = sizeof(radio_buffer);

/*================================= public ==================================*/

int main(void) {
  /* Initialize the board */
  board.init();

  /* Enable the SPI interface */
  spi0.enable(SPI_BAUDRATE);

  /* Enable the UART interface */
  uart0.enable(UART_BAUDRATE);

  /* Initialize Serial interface */
  serial.init();

  /* Initialize the DMA */
  dma.init();

  /* Start the scheduler */
  Scheduler::run();
  return 0;
}

/*================================ private ==================================*/

static void prvRadioTask(void *pvParameters) {
  bool status;

  /* Turn AT86RF215 radio on */
  at86rf215.on();

  /* Check AT86RF215 radio */
  status = at86rf215.check();
  if (!status) {
    /* Blink red LED */
    board.error();
  }

  /* Set radio callbacks and enable interrupts */
  at86rf215.setRxCallbacks(RADIO_CORE, &radio_rx_init_cb, &radio_rx_done_cb);
  at86rf215.enableInterrupts();

  /* Wake up and configure radio */
  at86rf215.wakeup(RADIO_CORE);
  at86rf215.configure(RADIO_CORE, RADIO_SETTINGS, RADIO_FREQUENCY, RADIO_CHANNEL);

  at86rf215.setTransmitPower(RADIO_CORE, RADIO_TX_POWER);

  /* Forever */
  while (true) {
    At86rf215::RadioResult result;
    int8_t rssi, lqi;
    bool crc;
    bool received;

    /* Initialize packet pointer and length */
    uint8_t *packet_ptr = radio_buffer;
    uint16_t packet_len = radio_buffer_len;

    /* while (true)
    {
      // Turn on yellow LED for 100 ms
      led_yellow.on();
      Scheduler::delay_ms(100);

      // Turn off yellow LED for 900 ms
      led_yellow.off();
      Scheduler::delay_ms(900);
    }*/

    /* Try to receive a packet */

    at86rf215.receive(RADIO_CORE);

    /* Wait until packet has been received */
    received = semaphore.take();

    /* If we have received a packet */
    if (received == true) {
      /* Load packet to radio */
      result = at86rf215.getPacket(RADIO_CORE, packet_ptr, &packet_len, &rssi, &lqi, &crc);

      /* Check packet has been received successfully */
      if (result == At86rf215::RadioResult::Success && crc == true) {
        uint16_t length;

        /* Turn on yellow LED */
        led_yellow.on();

        /* Prepare serial buffer */
        length = prepare_serial(serial_buffer, packet_ptr, packet_len, lqi);

        /* Send packet via Serial */
        serial.write(serial_buffer, length, true);

        /* Turn off yellow LED */
        led_yellow.off();
      }
    } else {
      /* Blink red LED */
      led_red.on();
      Scheduler::delay_ms(1);
      led_red.off();
    }
  }

  /* Turn AT86RF215 radio off */
  at86rf215.off();
}

static void prvGreenLedTask(void *pvParameters) {
  /* Forever */
  while (true) {
    /* Turn on green LED for 100 ms */
    led_green.on();
    Scheduler::delay_ms(100);

    /* Turn off green LED for 900 ms */
    led_green.off();
    Scheduler::delay_ms(900);
  }
}

static void radio_rx_init(void) {
  /* Turn on orange LED */
  led_orange.on();
}

static void radio_rx_done(void) {
  /* Turn off orange LED */
  led_orange.off();

  /* Notify we have transmitted a packet */
  semaphore.giveFromInterrupt();
}

static uint16_t prepare_serial(uint8_t *buffer_ptr, uint8_t *packet_ptr, uint16_t packet_length, int8_t lqi) {
  uint16_t length;

  /* Copy radio packet payload */
  dma.memcpy(buffer_ptr, packet_ptr, packet_length);

  /* Update buffer length */
  length = packet_length;

  /* Copy RSSI value */
  buffer_ptr[length++] = lqi;

  // Signaling byte
  buffer_ptr[length++] = 105;

  return length;
}
