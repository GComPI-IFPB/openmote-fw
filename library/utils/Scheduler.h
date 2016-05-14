/**
 * @file       Scheduler.h
 * @author     Pere Tuset-Peiro (peretuset@openmote.com)
 * @version    v0.1
 * @date       May, 2016
 * @brief
 *
 * @copyright  Copyright 2015, OpenMote Technologies, S.L.
 *             This file is licensed under the GNU General Public License v2.
 */

#ifndef SCHEDULER_H_
#define SCHEDULER_H_

#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"

class Scheduler
{
public:
    Scheduler();
    static void run();
private:
};

#endif /* SCHEDULER_H_ */
