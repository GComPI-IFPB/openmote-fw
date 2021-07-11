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

#include "platform_types.hpp"

#include "Gpio.hpp"
#include "I2c.hpp"
#include "Spi.hpp"

#include "Callback.hpp"
#include "Scheduler.hpp"
#include "Semaphore.hpp"
#include "Task.hpp"

#include "Serial.hpp"

#include "At86rf215.hpp"
#include "At86rf215_conf.h"

#include "Bme280.hpp"
#include "Opt3001.hpp"

/*================================ define ===================================*/

#define HEARTBEAT_TASK_PRIORITY (tskIDLE_PRIORITY + 0)
#define TRANSMIT_TASK_PRIORITY (tskIDLE_PRIORITY + 1)

#define HEARTBEAT_TASK_STACK_SIZE (128)
#define TRANSMIT_TASK_STACK_SIZE (1024)

#define SPI_BAUDRATE (8000000)
#define UART_BAUDRATE (115200)

#define TX_BUFFER_LENGTH (127)
#define EUI48_ADDDRESS_LENGTH (6)

#define RADIO_BUFFER_LENGTH (1024)
#define SERIAL_BUFFER_LENGTH (1024)

#define SENSORS_CTRL_PORT (GPIO_A_BASE)
#define SENSORS_CTRL_PIN (GPIO_PIN_7)

#define BME280_I2C_ADDRESS (BME280_I2C_ADDR_PRIM)
#define OPT3001_I2C_ADDRESS (OPT3001_I2C_ADDR_GND)

#define RADIO_CORE (At86rf215::CORE_RF09)
// #define RADIO_SETTINGS (&radio_settings[CONFIG_OFDM2_MCS0])
// #define RADIO_FREQUENCY (&frequency_settings_09[FREQUENCY_09_OFDM2])

#define OFDM_SETTINGS (&radio_settings[CONFIG_OFDM2_MCS0])          /* BPSK,   rate 1/2, 4x repetition,   50 kbps */
#define OFDM_FREQUENCY (&frequency_settings_09[FREQUENCY_09_OFDM2]) /* OFDM Mode 2,  800 kHz */

#define FSK_SETTINGS (&radio_settings[CONFIG_FSK_OPTION1])        /* X 2-FSK,  50 kbps */
#define FSK_FREQUENCY (&frequency_settings_09[FREQUENCY_09_FSK1]) /* FSK Mode 1,   200 kHz */

#define OQPSK_SETTINGS (&radio_settings[CONFIG_OQPSK_RATE4])         /* X OQPSK-DSSS,  100 kchips/s,  50.00 kbps */
#define OQPSK_FREQUENCY (&frequency_settings_09[FREQUENCY_09_OQPSK]) /* OQPSK,        600 kHz */

#define RADIO_CHANNEL (0)
#define RADIO_TX_POWER (At86rf215::TransmitPower::TX_POWER_MAX)

#define NODE_ADDR 'a'

/*================================ typedef ==================================*/

/*=============================== prototypes ================================*/

extern "C" void board_sleep(TickType_t xModifiableIdleTime);
extern "C" void board_wakeup(TickType_t xModifiableIdleTime);

static void prvHeartbeatTask(void *pvParameters);
static void prvTransmitTask(void *pvParameters);
static void prvLogTask(void *pvParameters);

static uint16_t prepare_packet(uint8_t *packet_ptr, uint32_t packet_counter, uint8_t tx_mode, uint8_t tx_counter, uint8_t csma_retries, int8_t csma_rssi);

static void radio_rx_init(void);
static void radio_rx_done(void);
static void radio_tx_init(void);
static void radio_tx_done(void);

static uint16_t prepare_serial(uint8_t *buffer_ptr, uint8_t *rx_packet_ptr, uint16_t packet_length, int8_t lqi);

/*=============================== variables =================================*/

static Serial serial(uart0);

static Task heartbeatTask{(const char *)"Heartbeat", 128, (unsigned char)HEARTBEAT_TASK_PRIORITY, prvHeartbeatTask, nullptr};
static Task radioTask{(const char *)"Transmit", 128, (unsigned char)TRANSMIT_TASK_PRIORITY, prvTransmitTask, nullptr};
static Task logTask{(const char *)"Log", 128, (unsigned char)TRANSMIT_TASK_PRIORITY, prvLogTask, nullptr};

static PlainCallback radio_rx_init_cb{&radio_rx_init};
static PlainCallback radio_rx_done_cb{&radio_rx_done};
static PlainCallback radio_tx_init_cb{&radio_tx_init};
static PlainCallback radio_tx_done_cb{&radio_tx_done};

static SemaphoreBinary tx_semaphore{false};
static SemaphoreBinary rx_semaphore{false};

static uint8_t serial_buffer[SERIAL_BUFFER_LENGTH];

static uint8_t radio_buffer[TX_BUFFER_LENGTH];
static uint8_t eui48_address[EUI48_ADDDRESS_LENGTH];

static uint8_t radio_ack_buffer[RADIO_BUFFER_LENGTH];
static uint16_t radio_ack_buffer_len = sizeof(radio_ack_buffer);

static bool board_slept;

  uint8_t *ack_ptr;
  uint16_t ack_len;
  At86rf215::RadioResult result;
  int8_t rssi, lqi;
  bool crc;

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

static void prvLogTask(void *pvParameters) {
	while(true) {
		rx_semaphore.take();
		uint16_t length = prepare_serial(serial_buffer, ack_ptr, ack_len, lqi);
     	serial.write(serial_buffer, length, true);
	}
}

/*================================ private ==================================*/

static void prvTransmitTask(void *pvParameters) {
  uint32_t packet_counter = 0;
  uint8_t tx_mode = 0;
  uint8_t cycle = 0;
  int8_t cca_threshold = 0;
  uint8_t csma_retries = 0;
  int8_t csma_rssi = 0;
  bool csma_check = false;

  bool received;
  bool taken;

  /* Set radio callbacks and enable interrupts */
  at86rf215.setTxCallbacks(RADIO_CORE, &radio_tx_init_cb, &radio_tx_done_cb);
  at86rf215.setRxCallbacks(RADIO_CORE, &radio_rx_init_cb, &radio_rx_done_cb);
  at86rf215.enableInterrupts();

  /* Forever */
  while (true) {
    uint16_t tx_buffer_len;

    // Sensors delay
    Scheduler::delay_ms(100);

    bool sent;

    for (cycle = 0; cycle < 3; cycle++) {

      if (cycle == 1) {
        Scheduler::delay_ms(100);
      }
      if (cycle == 2) {
        Scheduler::delay_ms(200);
      }

      for (tx_mode = 0; tx_mode < 3; tx_mode++) {
        /* Turn AT86RF215 radio off */
        at86rf215.on();

        /* Wake up and configure radio */
        at86rf215.wakeup(RADIO_CORE);

        // Run through 3 pre configured radio settings
        switch (tx_mode) {
        case 0:
          // Configure FSK Radio
          at86rf215.configure(RADIO_CORE, FSK_SETTINGS, FSK_FREQUENCY, RADIO_CHANNEL);
          cca_threshold = -94;

          break;
        case 1:
          // Rádio OQPSK
          at86rf215.configure(RADIO_CORE, OQPSK_SETTINGS, OQPSK_FREQUENCY, RADIO_CHANNEL);
          cca_threshold = -93;

          break;
        case 2:
          // Configure OFDM Radio
          at86rf215.configure(RADIO_CORE, OFDM_SETTINGS, OFDM_FREQUENCY, RADIO_CHANNEL);
          cca_threshold = -91;

          break;
        default:
          at86rf215.configure(RADIO_CORE, OFDM_SETTINGS, OFDM_FREQUENCY, RADIO_CHANNEL);
          break;
        }

        /* Set Tx Power to the maximum */
        at86rf215.setTransmitPower(RADIO_CORE, RADIO_TX_POWER);

        // Check if channel is busy
        csma_check = at86rf215.csma(RADIO_CORE, cca_threshold, &csma_retries, &csma_rssi);

        /* Transmit packet if the channel is free */
        if (csma_check) {
          /* Prepare radio packet */
          tx_buffer_len = prepare_packet(radio_buffer, packet_counter, tx_mode, cycle, csma_retries, csma_rssi);

          /* Load packet to radio */
          at86rf215.loadPacket(RADIO_CORE, radio_buffer, tx_buffer_len);

          at86rf215.transmit(RADIO_CORE);

          /* Wait until packet has been transmitted */
          sent = tx_semaphore.take();

          //TODO: MANDAR PELA SERIAL OS DADOS DO PACOTE DE DADOS TRANSMITIDO

          at86rf215.receive(RADIO_CORE);

          /* Wait for the ACK */
          Scheduler::delay_ms(25);

          /* Turn AT86RF215 radio off */
          at86rf215.off();
          led_red.off();
          Scheduler::delay_ms(25);
        } else {
          /* Turn AT86RF215 radio off */
          //TODO: MANDAR PELA SERIAL OS DADOS DO PACOTE DE DADOS NÃO TRANSMITIDO
          at86rf215.off();
          Scheduler::delay_ms(50);
        }
      }
    }

    /* Increment packet counter */
    packet_counter++;

    // Delay
    Scheduler::delay_ms(5825);
  }
}

static void prvHeartbeatTask(void *pvParameters) {
  /* Forever */
  while (true) {
    /* Turn on green LED for 10 ms */
    led_green.on();
    Scheduler::delay_ms(10);

    /* Turn off green LED for 990 ms */
    led_green.off();
    Scheduler::delay_ms(990);
  }
}

static void radio_rx_init(void) {
  /* Turn on orange LED */
  led_red.on();
}

static void radio_rx_done(void) {
  /* Turn off orange LED */

  ack_ptr = radio_ack_buffer;
  ack_len = sizeof(radio_ack_buffer);
 
  result = at86rf215.getPacket(RADIO_CORE, ack_ptr, &ack_len, &rssi, &lqi, &crc);
  if (result == At86rf215::RadioResult::Success && crc == true && ack_ptr[2] == NODE_ADDR) {
     /* Notify we have received a packet */
  	 rx_semaphore.giveFromInterrupt();

    //length = prepare_serial(serial_buffer, ack_ptr, ack_len, lqi);
  //  serial.write(serial_buffer, length, true);
  }
  else {
  	led_red.off();
  }


}

static void radio_tx_init(void) {
  /* Turn on orange LED */
  led_yellow.on();
}

static void radio_tx_done(void) {
  /* Turn off orange LED */
  led_yellow.off();

  /* Notify we have transmitted a packet */
  tx_semaphore.giveFromInterrupt();
}

void board_sleep(TickType_t xModifiableIdleTime) {
  /* Check if board can go to sleep */
  if (i2c.canSleep()) {
    /* If so, put SPI & I2C to sleep */
    i2c.sleep();

    /* Remember that the board went to sleep */
    board_slept = true;
  } else {
    /* If not, remember that the board did NOT went to sleep */
    board_slept = false;

    /* And update the time to ensure it does NOT got to sleep */
    xModifiableIdleTime = 0;
  }
}

void board_wakeup(TickType_t xModifiableIdleTime) {
  /* Check if the board went to sleep */
  if (board_slept) {
    /* If so, wakeup SPI & I2C */
    i2c.wakeup();
  }
}

static uint16_t prepare_packet(uint8_t *packet_ptr, uint32_t packet_counter, uint8_t tx_mode, uint8_t tx_counter, uint8_t csma_retries, int8_t csma_rssi) {
  uint16_t packet_length = 0;

  // Signaling byte
  packet_ptr[0] = 73;

  /* Copy MAC address */
  for (packet_length = 1; packet_length < EUI48_ADDDRESS_LENGTH + 1; packet_length++) {
    packet_ptr[packet_length] = NODE_ADDR;
  }

  /* Copy packet counter */
  packet_ptr[packet_length++] = (uint8_t)((packet_counter & 0xFF000000) >> 24);
  packet_ptr[packet_length++] = (uint8_t)((packet_counter & 0x00FF0000) >> 16);
  packet_ptr[packet_length++] = (uint8_t)((packet_counter & 0x0000FF00) >> 8);
  packet_ptr[packet_length++] = (uint8_t)((packet_counter & 0x000000FF) >> 0);

  // Tx info
  packet_ptr[packet_length++] = tx_mode;
  packet_ptr[packet_length++] = tx_counter;

  // CSMA info
  packet_ptr[packet_length++] = csma_retries;
  packet_ptr[packet_length++] = csma_rssi;

  // Fill 32 bytes
  packet_ptr[packet_length++] = 0; // 16;
  packet_ptr[packet_length++] = 0; // 17;
  packet_ptr[packet_length++] = 0; // 18;
  packet_ptr[packet_length++] = 0; // 19;
  packet_ptr[packet_length++] = 0; // 20;
  packet_ptr[packet_length++] = 0; // 21;
  packet_ptr[packet_length++] = 0; // 22;
  packet_ptr[packet_length++] = 0; // 23;
  packet_ptr[packet_length++] = 0; // 24;
  packet_ptr[packet_length++] = 0; // 25;
  packet_ptr[packet_length++] = 0; // 26;
  packet_ptr[packet_length++] = 0; // 27;
  packet_ptr[packet_length++] = 0; // 28;
  packet_ptr[packet_length++] = 0; // 29;
  packet_ptr[packet_length++] = 0; // 30;
  packet_ptr[packet_length++] = 0; // 31;
  packet_ptr[packet_length++] = 0; // 32;

  return packet_length;
}

static uint16_t prepare_serial(uint8_t *buffer_ptr, uint8_t *rx_packet_ptr, uint16_t packet_length, int8_t lqi) {
  uint16_t length;

  /* Copy radio packet payload */
  dma.memcpy(buffer_ptr, rx_packet_ptr,packet_length);

  /* Update buffer length */
  length = packet_length;

  /* Copy RSSI value */
  buffer_ptr[length++] = lqi;

  // Signaling byte
  buffer_ptr[length++] = 105;

  return length;
}
