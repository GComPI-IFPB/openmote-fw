/**
 * @file       SerialParser.hpp
 * @author     Pere Tuset-Peiro (peretuset@openmote.com)
 * @version    v0.1
 * @date       January, 2019
 * @brief
 *
 * @copyright  Copyright 2019, OpenMote Technologies, S.L.
 *             This file is licensed under the GNU General Public License v2.
 */

#ifndef SERIAL_PARSER_HPP_
#define SERIAL_PARSER_HPP_

#include <stdint.h>
#include <stdbool.h>

#include "Thread.hpp"

class Serial;

typedef enum
{
  CMD_ERROR     = 0x00,
  CMD_ON        = 0x01,
  CMD_OFF       = 0x02,
  CMD_RST       = 0x03,
  CMD_CFG       = 0x04,
  CMD_RX        = 0x05,
  CMD_TX        = 0x06,
  CMD_TX_INTERF = 0x07,
} SerialCommand;

typedef enum
{
  RSLT_ERROR   = 0x00,
  RSLT_SUCCESS = 0x01
} SerialResult;

typedef struct
{
  SerialCommand command;
} SerialRequest;

typedef struct
{
  SerialResult result;
} SerialResponse;

typedef struct
{
  bool (* on)(uint8_t rc);
  bool (* off)(uint8_t rc);
  bool (* reset)(uint8_t rc);
  bool (* config)(uint8_t rc, uint8_t settings, uint8_t frequency, uint8_t length, uint8_t power);
  bool (* receive)(uint8_t rc, uint8_t timeout_ms);
  bool (* transmit)(uint8_t rc);
  bool (* transmit_continuous)(uint8_t rc, bool enable);
} SerialCallbacks;

class SerialParser : protected Thread
{
public:
  SerialParser(const char* const name, const uint16_t size, uint8_t priority,
               Serial& serial, SerialCallbacks& callbacks);
protected:
  void run(void);
private:
  bool parse_on(uint8_t* params);
  bool parse_off(uint8_t* params);
  bool parse_reset(uint8_t* params);
  bool parse_config(uint8_t* params);
  bool parse_receive(uint8_t* params);
  bool parse_transmit(uint8_t* params);
  bool parse_transmit_continuous(uint8_t* params);
private:
  Serial& serial_;
  SerialCallbacks& callbacks_;
};

#endif /* SERIAL_PARSER_HPP_ */
