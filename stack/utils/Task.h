/**
 * @file       Task.h
 * @author     Pere Tuset-Peiro (peretuset@openmote.com)
 * @version    v0.1
 * @date       May, 2016
 * @brief
 *
 * @copyright  Copyright 2015, OpenMote Technologies, S.L.
 *             This file is licensed under the GNU General Public License v2.
 */

#ifndef TASK_H_
#define TASK_H_

#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"

class Task
{
public:
    Task(const char* const name, uint8_t priority);
    void delay(uint32_t milliseconds);
public:
    static void init(void* params);
protected:
	virtual void run(void) = 0;
private:
	const char* const name_;
	uint8_t priority_;
};

#endif /* TASK_H_ */
