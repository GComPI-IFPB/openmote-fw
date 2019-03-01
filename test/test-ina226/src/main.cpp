/**
 * @file       main.cpp
 * @author     Pere Tuset-Peiro (peretuset@openmote.com)
 * @version    v0.1
 * @date       May, 2015
 * @brief
 *
 * @copyright  Copyright 2015, OpenMote Technologies, S.L.
 *             This file is licensed under the GNU General Public License v2.
 */

/*================================ include ==================================*/

#include "BoardImplementation.hpp"

#include "I2c.hpp"

#include "Callback.hpp"
#include "Scheduler.hpp"
#include "Semaphore.hpp"
#include "Task.hpp"

#include "Ina226.hpp"
#include "Ina226_regs.h"

/*================================ define ===================================*/

#define GREEN_LED_TASK_PRIORITY         ( tskIDLE_PRIORITY + 0 )
#define INA226_TASK_PRIORITY            ( tskIDLE_PRIORITY + 1 )

#define INA226_IRQ_PORT                 ( GPIO_B_BASE )
#define INA226_IRQ_PIN                  ( GPIO_PIN_2 )
#define INA226_IRQ_IOC                  ( IOC_OVERRIDE_PUE )
#define INA226_IRQ_EDGE                 ( GPIO_FALLING_EDGE )
#define INA226_I2C_ADDRESS              ( 0x40 )

#define INA226_USER_CONFIG              ( INA226_CONFIG_DEFAULT | \
                                          INA226_CONFIG_AVG_BIT_1024 | INA226_CONFIG_CT_VSH_588US | \
                                          INA226_CONFIG_CT_VBUS_588US | INA226_CONFIG_MODE_SV_CONT )
#define INA226_USER_CALIB               ( 0x0000 )

/*================================ typedef ==================================*/

/*=============================== prototypes ================================*/

static void prvGreenLedTask(void *pvParameters);
static void prvIna226Task(void *pvParameters);

/*=============================== variables =================================*/

static Task heartbeatTask{(const char *) "Heartbeat", 128, GREEN_LED_TASK_PRIORITY, prvGreenLedTask, nullptr};
static Task ina226Task{(const char *) "Ina226", 128, INA226_TASK_PRIORITY, prvIna226Task, nullptr};

static const GpioConfig ina226_irq_cfg {INA226_IRQ_PORT, INA226_IRQ_PIN, INA226_IRQ_IOC, INA226_IRQ_EDGE, 0};
static GpioIn ina226_irq {ina226_irq_cfg};
static Ina226 ina226 {i2c, ina226_irq};

/*================================= public ==================================*/

int main (void)
{
  /* Initialize the board */
  board.init();
  
  /* Enable I2C */
  i2c.enable();

  /* Start the scheduler */
  Scheduler::run();
}

/*=============================== protected =================================*/

/*================================ private ==================================*/

static void prvIna226Task(void *pvParameters)
{
  Ina226Config config;
  Ina226Data data;
  bool status;
  
  config.config = INA226_USER_CONFIG;
  config.calibration = INA226_USER_CALIB;
  
  ina226.init(INA226_I2C_ADDRESS);
  ina226.reset();
  
  status = ina226.configure(config);
  
  status = ina226.start();
  
  /* Forever */
  while (true)
  {
    status = ina226.read(data, INA226_MEASURE_ALL, 1000);
    if (status)
    {
      led_yellow.on();
      Scheduler::delay_ms(10);
      led_yellow.off();
    }
    else
    {
      led_orange.on();
      Scheduler::delay_ms(10);
      led_orange.off();
    }
  }
}

static void prvGreenLedTask(void *pvParameters)
{
  /* Forever */
  while (true)
  {
    /* Turn off green LED for 950 ms */
    led_green.off();
    Scheduler::delay_ms(950);
    
    /* Turn on green LED for 50 ms */
    led_green.on();
    Scheduler::delay_ms(50);
  }
}
