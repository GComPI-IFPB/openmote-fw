/**
 * @file       SerialParser.cpp
 * @author     Pere Tuset-Peiro (peretuset@openmote.com)
 * @version    v0.1
 * @date       January, 2019
 * @brief
 *
 * @copyright  Copyright 2019, OpenMote Technologies, S.L.
 *             This file is licensed under the GNU General Public License v2.
 */

/*================================ include ==================================*/

#include "SerialParser.hpp"

#include "Scheduler.hpp"

#include "Serial.hpp"

/*================================ define ===================================*/

#define SERIAL_BUFFER_LENGTH            ( 256 )

/*================================ typedef ==================================*/

/*=============================== prototypes ================================*/

/*=============================== variables =================================*/

static uint8_t serial_rx_buffer[SERIAL_BUFFER_LENGTH];
static uint16_t serial_rx_buffer_len = sizeof(serial_rx_buffer);

static uint8_t serial_tx_buffer[SERIAL_BUFFER_LENGTH];
static uint16_t serial_tx_buffer_len = sizeof(serial_tx_buffer);

/*================================= public ==================================*/

SerialParser::SerialParser(const char* const name, const uint16_t size, uint8_t priority,
                           Serial& serial, SerialCallbacks& callbacks):
  Thread(name, size, priority), serial_(serial), callbacks_(callbacks)
{
}

void SerialParser::run(void)
{
  while(true)
  {
    uint16_t length;
    
    /* Read buffer using Serial */
    length = serial_.read(serial_rx_buffer, serial_rx_buffer_len);
    
    /* If we have received a message */
    if (length > 0)
    {
      SerialCommand cmd;
      uint8_t* params;
      bool result;
      
      cmd = (SerialCommand)serial_rx_buffer[0];
      params = &serial_rx_buffer[1];
      
      switch(cmd)
      {
        case CMD_ON:
          result = parse_on(params);
          break;
        case CMD_OFF:
          result = parse_off(params);
          break;
        case CMD_RST:
          result = parse_reset(params);
          break;
        case CMD_CFG:
          result = parse_config(params);
          break;
        case CMD_RX:
          result = parse_receive(params);
          break;
        case CMD_TX:
          result = parse_transmit(params);
          break;
        case CMD_TX_INTERF:
          result = parse_transmit_continuous(params);
          break;
        default:
          break;
      }
      
      /* Create message based on result */
      if (result)
      {
        serial_tx_buffer[0] = RSLT_SUCCESS;
        serial_tx_buffer_len = 1;
      }
      else
      {
        serial_tx_buffer[0] = RSLT_ERROR;
        serial_tx_buffer_len = 1;
      }
      
      /* Transmit result message */
      serial_.write(serial_tx_buffer, serial_tx_buffer_len, true);
    }
    
    Scheduler::delay_ms(1);
  }
}

/*================================ private ==================================*/

bool SerialParser::parse_on(uint8_t* params)
{
  bool result;
  uint8_t rc, mode;
  
  rc  = params[0];
  mode = params[1];
  
  result = callbacks_.on(rc, mode);
  
  return result;
}

bool SerialParser::parse_off(uint8_t* params)
{
  bool result;
  uint8_t rc;
  
  rc  = params[0];
  
  result = callbacks_.off(rc);
  
  return result;
}

bool SerialParser::parse_reset(uint8_t* params)
{
  bool result;
  uint8_t rc;
  
  rc  = params[0];
  
  result = callbacks_.reset(rc);
  
  return result;
}

bool SerialParser::parse_config(uint8_t* params)
{
  uint8_t rc, settings, frequency, length, power;
  bool result;
  
  rc        = params[0]; 
  settings  = params[1];
  frequency = params[2];
  length    = params[3];
  power     = params[4];
  
  result = callbacks_.config(rc, settings, frequency, length, power);
  
  return result;
}

bool SerialParser::parse_receive(uint8_t* params)
{
  bool result;
  uint8_t rc;
  uint8_t timeout_ms;
  
  rc = params[0];
  timeout_ms = params[1];
  
  result = callbacks_.receive(rc, timeout_ms);
  
  return result;
}

bool SerialParser::parse_transmit(uint8_t* params)
{
  bool result;
  uint8_t rc;
  
  rc  = params[0];
  
  result = callbacks_.transmit(rc);
  
  return result;
}

bool SerialParser::parse_transmit_continuous(uint8_t* params)
{
  bool result, enable;
  uint8_t rc;
  
  rc  = params[0];
  enable = (params[1] == 1);
  
  result = callbacks_.transmit_continuous(rc, enable);
  
  return result;
}