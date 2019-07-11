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

#include "At86rf215.hpp"
#include "At86rf215_conf.h"

#include "Bme280.hpp"
#include "Opt3001.hpp"

/*================================ define ===================================*/

#define HEARTBEAT_TASK_PRIORITY             ( tskIDLE_PRIORITY + 0 )
#define TRANSMIT_TASK_PRIORITY              ( tskIDLE_PRIORITY + 1 )

#define HEARTBEAT_TASK_STACK_SIZE           ( 128 )
#define TRANSMIT_TASK_STACK_SIZE            ( 1024 )

#define SPI_BAUDRATE                        ( 16000000 )

#define TX_BUFFER_LENGTH                    ( 127 )
#define EUI48_ADDDRESS_LENGTH               ( 6 )

#define SENSORS_CTRL_PORT                   ( GPIO_A_BASE )
#define SENSORS_CTRL_PIN                    ( GPIO_PIN_7 )

#define BME280_I2C_ADDRESS                  ( BME280_I2C_ADDR_PRIM )
#define OPT3001_I2C_ADDRESS                 ( OPT3001_I2C_ADDR_GND )

#define RADIO_CORE                          ( At86rf215::CORE_RF09 )
#define RADIO_SETTINGS                      ( &radio_settings[CONFIG_OFDM2_MCS0] )
#define RADIO_FREQUENCY                     ( &frequency_settings_09[FREQUENCY_09_OFDM2] )
#define RADIO_CHANNEL                       ( 0 )
#define RADIO_TX_POWER                      ( At86rf215::TransmitPower::TX_POWER_MAX )

/*================================ typedef ==================================*/

typedef struct {
  uint16_t temperature;
  uint16_t humidity;
  uint16_t pressure;
  uint16_t light;
} SensorData;

/*=============================== prototypes ================================*/

extern "C" void board_sleep(TickType_t xModifiableIdleTime);
extern "C" void board_wakeup(TickType_t xModifiableIdleTime);

static void prvHeartbeatTask(void *pvParameters);
static void prvTransmitTask(void *pvParameters);

static uint16_t prepare_packet(uint8_t* packet_ptr, uint8_t* eui48_address, uint32_t packet_counter, SensorData sensor_data);

static void radio_tx_init(void);
static void radio_tx_done(void);

/*=============================== variables =================================*/

static Task heartbeatTask{(const char *) "Heartbeat", 128, HEARTBEAT_TASK_PRIORITY, prvHeartbeatTask, nullptr};
static Task radioTask{(const char *) "Transmit", 128, TRANSMIT_TASK_PRIORITY, prvTransmitTask, nullptr};

static GpioConfig sensors_pwr_cfg = {SENSORS_CTRL_PORT, SENSORS_CTRL_PIN, 0, 0, 0};
static GpioOut sensors_pwr_ctrl {sensors_pwr_cfg};

static Bme280 bme280 {i2c, BME280_I2C_ADDRESS};
static Opt3001 opt3001 {i2c, OPT3001_I2C_ADDRESS};

static PlainCallback radio_tx_init_cb {&radio_tx_init};
static PlainCallback radio_tx_done_cb {&radio_tx_done};

static SemaphoreBinary semaphore {false};

static uint8_t radio_buffer[TX_BUFFER_LENGTH];
static uint8_t eui48_address[EUI48_ADDDRESS_LENGTH];

static bool board_slept;

/*================================= public ==================================*/

void main(void)
{
  /* Initialize the board */
  board.init();
  
  /* Enable the SPI interface */
  spi0.enable(SPI_BAUDRATE);
  
  /* Enable the I2C interface */
  i2c.enable();
  
  /* Turn on the sensors board */
  sensors_pwr_ctrl.high();

  /* Start the scheduler */
  Scheduler::run();
}

/*================================ private ==================================*/

static void prvTransmitTask(void *pvParameters)
{
  uint16_t packet_counter = 0;
	bool status; 
  
  /* Get EUI48 address */
  board.getEUI48(eui48_address);
  
  /* Initialize BME280 and OPT3001 sensors */
  bme280.init();
  opt3001.init();
  opt3001.enable();
  
  /* Set radio callbacks and enable interrupts */
  at86rf215.setTxCallbacks(RADIO_CORE, &radio_tx_init_cb, &radio_tx_done_cb);
  at86rf215.enableInterrupts();
  
  /* Forever */
  while (true)
  {
    SensorData sensor_data;
    Bme280Data bme280_data;
    Opt3001Data opt3001_data;
    uint16_t tx_buffer_len;
    bool status;

    /* Turn on red LED */
    led_red.on();

    /* Read temperature, humidity and pressure */
    status = bme280.read(&bme280_data);
    if (!status)
    {
      /* Reset BME280 */
      bme280.reset();

      /* Re-initialize BME280 */
      bme280.init();
    }

    /* Read light */
    status = opt3001.read(&opt3001_data.raw);
    if (status)
    {
      opt3001.convert(opt3001_data.raw, &opt3001_data.lux);
    }
    
    /* Turn off red LED */
    led_red.off();

    /* Convert sensor data */
    if (status)
    {
      bool sent;
      
      /* Fill-in sensor data */
      sensor_data.temperature = (uint16_t) (bme280_data.temperature * 10.0f);
      sensor_data.humidity    = (uint16_t) (bme280_data.humidity * 10.0f);
      sensor_data.pressure    = (uint16_t) (bme280_data.pressure * 10.0f);
      sensor_data.light       = (uint16_t) (opt3001_data.lux * 10.0f);
      
      /* Turn AT86RF215 radio off */
      at86rf215.on();
      
      /* Wake up and configure radio */
      at86rf215.wakeup(RADIO_CORE);
      at86rf215.configure(RADIO_CORE, RADIO_SETTINGS, RADIO_FREQUENCY, RADIO_CHANNEL);
      at86rf215.setTransmitPower(RADIO_CORE, RADIO_TX_POWER);

      /* Prepare radio packet */
      tx_buffer_len = prepare_packet(radio_buffer, eui48_address, packet_counter, sensor_data);
      
      /* Load packet to radio */
      at86rf215.loadPacket(RADIO_CORE, radio_buffer, tx_buffer_len);
          
      /* Transmit packet */
      at86rf215.transmit(RADIO_CORE);

      /* Wait until packet has been transmitted */
      sent = semaphore.take();
      
      /* Turn AT86RF215 radio off */
      at86rf215.off();
    }

    /* Increment packet counter */
    packet_counter++;

    /* Delay for 10 seconds */
    Scheduler::delay_ms(10000);
  }
}

static void prvHeartbeatTask(void *pvParameters)
{
  /* Forever */
  while (true)
  {
    /* Turn on green LED for 10 ms */
    led_green.on();
    Scheduler::delay_ms(10);
    
    /* Turn off green LED for 990 ms */
    led_green.off();
    Scheduler::delay_ms(990);
  }
}

static void radio_tx_init(void)
{
  /* Turn on orange LED */
  led_orange.on();
}

static void radio_tx_done(void)
{
  /* Turn off orange LED */
  led_orange.off();

  /* Notify we have transmitted a packet */
  semaphore.giveFromInterrupt();
}

void board_sleep(TickType_t xModifiableIdleTime)
{
  /* Check if board can go to sleep */
  if (i2c.canSleep())
  {
    /* If so, put SPI & I2C to sleep */
    i2c.sleep();

    /* Remember that the board went to sleep */
    board_slept = true;
  }
  else
  {
    /* If not, remember that the board did NOT went to sleep */
    board_slept = false;

    /* And update the time to ensure it does NOT got to sleep */
    xModifiableIdleTime = 0;
  }
}

void board_wakeup(TickType_t xModifiableIdleTime)
{
  /* Check if the board went to sleep */
  if (board_slept)
  {
    /* If so, wakeup SPI & I2C */
    i2c.wakeup();
  }
}

static uint16_t prepare_packet(uint8_t* packet_ptr, uint8_t* eui48_address, uint32_t packet_counter, SensorData sensor_data)
{
  uint16_t packet_length = 0;
  uint8_t i;

  /* Copy MAC adress */
  for (i = 0; i < EUI48_ADDDRESS_LENGTH; i++)
  {
    packet_ptr[i] = eui48_address[i];
  }
  packet_length = i;

  /* Copy packet counter */
  i = packet_length;
  packet_ptr[i++] = (uint8_t)((packet_counter & 0xFF000000) >> 24);
  packet_ptr[i++] = (uint8_t)((packet_counter & 0x00FF0000) >> 16);
  packet_ptr[i++] = (uint8_t)((packet_counter & 0x0000FF00) >> 8);
  packet_ptr[i++] = (uint8_t)((packet_counter & 0x000000FF) >> 0);
  packet_length = i;

  /* Copy sensor data */
  i = packet_length;
  packet_ptr[i++] = (uint8_t) ((sensor_data.temperature & 0xFF00) >> 8);
  packet_ptr[i++] = (uint8_t) ((sensor_data.temperature & 0x00FF) >> 0);
  packet_ptr[i++] = (uint8_t) ((sensor_data.humidity & 0xFF00) >> 8);
  packet_ptr[i++] = (uint8_t) ((sensor_data.humidity & 0x00FF) >> 0);
  packet_ptr[i++] = (uint8_t) ((sensor_data.pressure & 0xFF00) >> 8);
  packet_ptr[i++] = (uint8_t) ((sensor_data.pressure & 0x00FF) >> 0);
  packet_ptr[i++] = (uint8_t) ((sensor_data.light & 0xFF00) >> 8);
  packet_ptr[i++] = (uint8_t) ((sensor_data.light & 0x00FF) >> 0);
  packet_length = i;

  return packet_length;
}