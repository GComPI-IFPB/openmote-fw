/**
 * @file       Logging.h
 * @author     Pere Tuset-Peiro (peretuset@openmote.com)
 * @version    v0.1
 * @date       May, 2016
 * @brief
 *
 * @copyright  Copyright 2016, OpenMote Technologies, S.L.
 *             This file is licensed under the GNU General Public License v2.
 */

#ifndef LOGGING_H_
#define LOGGING_H_

#include <stdint.h>

#include "Callback.h"
#include "CircularBuffer.h"
#include "LinkedList.h"
#include "Mutex.h"

class Logging;
class Uart;

enum LogLevel {
	None,
	Call,
	Info,
	Warn,
	Error,
	All
};

typedef GenericCallback<Logging> LoggingCallback;

class Logging : public Item<Logging>
{

friend class LoggingManager;

public:
	Logging(Uart& uart);
	void init(void);
	void setLevel(LogLevel level);
	void log(char const * message);
	void call(char const * message);
	void info(char const * message);
	void warn(char const * message);
	void error(char const * message);
private:
	void txCallback(void);
private:
	Uart& uart_;

	Mutex mutex_;
	LoggingCallback txCallback_;

	uint8_t transmit_buffer_[256];
    CircularBuffer txBuffer_;
	
	LogLevel level_;

	bool initialized_;
};

#endif /* LOGGING_H_ */