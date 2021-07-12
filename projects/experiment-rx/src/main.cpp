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
#define OFDM_SETTINGS (&radio_settings[CONFIG_OFDM2_MCS0])          /* BPSK,   rate 1/2, 4x repetition,   50 kbps */
#define OFDM_FREQUENCY (&frequency_settings_09[FREQUENCY_09_OFDM2]) /* OFDM Mode 2,  800 kHz */

/* FSK Radio settings */
#define FSK_SETTINGS (&radio_settings[CONFIG_FSK_OPTION1])        /* X 2-FSK,  50 kbps */
#define FSK_FREQUENCY (&frequency_settings_09[FREQUENCY_09_FSK1]) /* FSK Mode 1,   200 kHz */

/* OQPSK Radio settings */
#define OQPSK_SETTINGS (&radio_settings[CONFIG_OQPSK_RATE4])         /* X OQPSK-DSSS,  100 kchips/s,  50.00 kbps */
#define OQPSK_FREQUENCY (&frequency_settings_09[FREQUENCY_09_OQPSK]) /* OQPSK,        600 kHz */

#define RADIO_CHANNEL (0)
#define RADIO_TX_POWER (At86rf215::TransmitPower::TX_POWER_MAX)

#define EUI48_ADDDRESS_LENGTH (6)

/*================================ typedef ==================================*/

enum RadioMode {
  RadioMode_Transmit = 0x30,
  RadioMode_Receive = 0x31
};

/*=============================== prototypes ================================*/

static void prvGreenLedTask(void *pvParameters);
static void prvRadioTask(void *pvParameters);

static void radio_rx_init(void);
static void radio_rx_done(void);
static void radio_tx_init(void);
static void radio_tx_done(void);

static uint16_t prepare_serial(uint8_t *buffer_ptr, uint8_t *rx_packet_ptr, uint16_t packet_length, int8_t lqi);
static uint16_t prepare_ack_packet(uint8_t *packet_ptr, uint8_t node_id, uint8_t *packet_counter, uint8_t tx_mode, uint8_t tx_counter, uint8_t csma_retries, int8_t csma_rssi);

/*=============================== variables =================================*/

static Serial serial(uart0);

static Task heartbeatTask{(const char *)"Green", 128, (unsigned char)GREEN_LED_TASK_PRIORITY, prvGreenLedTask, nullptr};
static Task radioTask{(const char *)"Radio", 128, (unsigned char)RADIO_TASK_PRIORITY, prvRadioTask, nullptr};

static PlainCallback radio_rx_init_cb(&radio_rx_init);
static PlainCallback radio_rx_done_cb(&radio_rx_done);
static PlainCallback radio_tx_init_cb(&radio_tx_init);
static PlainCallback radio_tx_done_cb(&radio_tx_done);

static SemaphoreBinary rx_semaphore(false);
static SemaphoreBinary tx_semaphore(false);

static uint8_t serial_buffer[SERIAL_BUFFER_LENGTH];

static uint8_t radio_buffer[RADIO_BUFFER_LENGTH];
static uint16_t radio_buffer_len = sizeof(radio_buffer);

static uint8_t radio_ack_buffer[RADIO_BUFFER_LENGTH];
static uint16_t radio_ack_buffer_len = sizeof(radio_buffer);

static RadioMode radioMode;

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
  uint8_t *ack_ptr = radio_ack_buffer;
  uint16_t ack_len = sizeof(radio_ack_buffer);

  int8_t cca_threshold = 0;
  uint8_t csma_retries = 0;
  int8_t csma_rssi = 0;
  bool csma_check = false;

  /* Turn AT86RF215 radio on */
  at86rf215.on();

  /* Check AT86RF215 radio */
  status = at86rf215.check();
  if (!status) {
    /* Blink red LED */
    board.error();
  }

  /* Set radio callbacks and enable interrupts */
  at86rf215.setTxCallbacks(RADIO_CORE, &radio_tx_init_cb, &radio_tx_done_cb);
  at86rf215.setRxCallbacks(RADIO_CORE, &radio_rx_init_cb, &radio_rx_done_cb);
  at86rf215.enableInterrupts();

  /* Wake up and configure radio */
  at86rf215.wakeup(RADIO_CORE);
  at86rf215.configure(RADIO_CORE, FSK_SETTINGS, FSK_FREQUENCY, RADIO_CHANNEL);
  at86rf215.setTransmitPower(RADIO_CORE, RADIO_TX_POWER);
  cca_threshold = -94; //Threshold for FSK
  //cca_threshold = -93; //Threshold for OQPSK
  //cca_threshold = -91; //Threshold for OFDM
  
   /* Forever */
  while (true) {
    At86rf215::RadioResult result;
    int8_t rssi, lqi;
    uint8_t id;
    bool crc;
    bool received;
    bool taken;

    /* Initialize packet pointer and length */
    uint8_t *rx_packet_ptr = radio_buffer;
    uint16_t packet_len = radio_buffer_len;

    /* Try to receive a packet */
    at86rf215.receive(RADIO_CORE);

    /* Wait until packet has been received */
    received = rx_semaphore.take();

    /* If we have received a packet */
    if (received == true) {
      /* Load packet to radio */
      result = at86rf215.getPacket(RADIO_CORE, rx_packet_ptr, &packet_len, &rssi, &lqi, &crc);

      /* Check packet has been received successfully */
      if (result == At86rf215::RadioResult::Success && crc == true && rx_packet_ptr[15] == 0) {
        uint16_t length;

        /* Turn on yellow LED */
        // led_yellow.on();

        /* Prepare serial buffer */
        length = prepare_serial(serial_buffer, rx_packet_ptr, packet_len, lqi);

        /* Send packet via Serial */
        serial.write(serial_buffer, length, true);

        // -------------------- Send ACK Packet --------------------
        // at86rf215.configure(RADIO_CORE, OFDM_SETTINGS, OFDM_FREQUENCY, RADIO_CHANNEL);

        at86rf215.off();
        at86rf215.on();
        at86rf215.wakeup(RADIO_CORE);
        at86rf215.configure(RADIO_CORE, FSK_SETTINGS, FSK_FREQUENCY, RADIO_CHANNEL);
        at86rf215.setTransmitPower(RADIO_CORE, RADIO_TX_POWER);

        /* Wait 1ms to transmit the ACK */
        Scheduler::delay_ms(1);

        radioMode = RadioMode_Transmit;

        ack_len = 0;
        id = *((uint8_t *)(&rx_packet_ptr[1]));
        uint8_t *packet_counter = &rx_packet_ptr[7];
        uint8_t tx_mode = *((uint8_t *)(&rx_packet_ptr[11]));
        uint8_t cycle = *((uint8_t *)(&rx_packet_ptr[12]));

        // Check if channel is busy
        csma_check = at86rf215.csma(RADIO_CORE, cca_threshold, &csma_retries, &csma_rssi);

        /* Transmit packet if the channel is free */
        if (csma_check) {
          /* Prepare radio packet */
          ack_len = prepare_ack_packet(ack_ptr, id, packet_counter, tx_mode, cycle, csma_retries, csma_rssi);

          length = dma.memcpy(radio_ack_buffer, ack_ptr, ack_len);

          /* Load packet to radio */
          at86rf215.loadPacket(RADIO_CORE, radio_ack_buffer, length);

          /* Transmit packet */
          at86rf215.transmit(RADIO_CORE);

          /* Wait until packet has been transmitted */
          taken = tx_semaphore.take();

          //TODO: MANDAR PELA SERIAL OS DADOS DO PACOTE ACK TRANSMITIDO
        } else {
          //TODO: MANDAR PELA SERIAL OS DADOS DO PACOTE ACK NÃO TRANSMITIDO
        }

        // -------------------- Send ACK Packet --------------------
      }
    } else {
      /* Blink red LED */
     // led_red.on();
     // Scheduler::delay_ms(1);
      //led_red.off();
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

  /* Notify we have received a packet */
  rx_semaphore.giveFromInterrupt();
}

static void radio_tx_init(void) {
  /* Turn on yellow LED */
  led_yellow.on();
}

static void radio_tx_done(void) {
  /* Turn off yellow LED */
  led_yellow.off();

  /* Notify we have transmitted a packet */
  tx_semaphore.giveFromInterrupt();
}

static uint16_t prepare_serial(uint8_t *buffer_ptr, uint8_t *rx_packet_ptr, uint16_t packet_length, int8_t lqi) {
  uint16_t length;

  /* Copy radio packet payload */
  dma.memcpy(buffer_ptr, rx_packet_ptr, packet_length);

  /* Update buffer length */
  length = packet_length;

  /* Copy RSSI value */
  buffer_ptr[length++] = lqi;

  // Signaling byte
  buffer_ptr[length++] = 105;

  return length;
}

static uint16_t prepare_ack_packet(uint8_t *packet_ptr, uint8_t node_id, uint8_t *packet_counter, uint8_t tx_mode, uint8_t tx_counter, uint8_t csma_retries, int8_t csma_rssi) {
  uint16_t packet_length = 0;

  // Signaling byte
  packet_ptr[0] = 73;

  /* Copy MAC address */
  for (packet_length = 1; packet_length < EUI48_ADDDRESS_LENGTH + 1; packet_length++) {
    packet_ptr[packet_length] = node_id;
  }

  /* Copy packet counter */
  packet_ptr[packet_length++] = packet_counter[0];//(uint8_t)((packet_counter & 0xFF000000) >> 24);
  packet_ptr[packet_length++] = packet_counter[1];//(uint8_t)((packet_counter & 0x00FF0000) >> 16);
  packet_ptr[packet_length++] = packet_counter[2];//(uint8_t)((packet_counter & 0x0000FF00) >> 8);
  packet_ptr[packet_length++] = packet_counter[3];//(uint8_t)((packet_counter & 0x000000FF) >> 0);

  // Tx info
  packet_ptr[packet_length++] = tx_mode;
  packet_ptr[packet_length++] = tx_counter;

  // CSMA info
  packet_ptr[packet_length++] = csma_retries;
  packet_ptr[packet_length++] = csma_rssi;
  packet_ptr[packet_length++] = 1;

  return packet_length;
}
