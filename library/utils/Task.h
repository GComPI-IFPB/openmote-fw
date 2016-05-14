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
    Task();
    static void delay(uint32_t milliseconds);
    static void remove(void);
private:
};

#endif /* TASK_H_ */
