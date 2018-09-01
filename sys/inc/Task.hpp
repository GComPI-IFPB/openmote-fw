/**
 * @file       Task.hpp
 * @author     Pere Tuset-Peiro (peretuset@openmote.com)
 * @version    v0.1
 * @date       May, 2016
 * @brief
 *
 * @copyright  Copyright 2015, OpenMote Technologies, S.L.
 *             This file is licensed under the GNU General Public License v2.
 */

#ifndef TASK_HPP_
#define TASK_HPP_

#include <stdint.h>

#include "FreeRTOS.h"
#include "task.h"

class Task
{
public:
    Task(const char* const name, const uint16_t size, uint8_t priority);
public:
    static void init(void* params);
    static void delay(uint32_t milliseconds);
    static void yield(void);
protected:
	virtual void run(void) = 0;
protected:
	const char* const name_;
	uint8_t priority_;
};

#endif /* TASK_HPP_ */
