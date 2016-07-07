 /**
 * @file       Task.cpp
 * @author     Pere Tuset-Peiro (peretuset@openmote.com)
 * @version    v0.1
 * @date       May, 2016
 * @brief
 *
 * @copyright  Copyright 2015, OpenMote Technologies, S.L.
 *             This file is licensed under the GNU General Public License v2.
 */

/*================================ include ==================================*/

#include "Task.h"

/*================================ define ===================================*/

/*================================ typedef ==================================*/

/*=============================== variables =================================*/

/*=============================== prototypes ================================*/

/*================================= public ==================================*/

Task::Task(const char* const name, uint8_t priority) :
	name_(name), priority_(priority)
{
	xTaskCreate(Task::init, name_, 128, this, priority_, NULL);
}

void Task::init(void* params)
{
	Task* task = (Task *) params;
	task->run();
}

void Task::delay(uint32_t milliseconds)
{
    vTaskDelay(milliseconds / portTICK_RATE_MS);
}

void Task::yield(void)
{
    taskYIELD();
}

/*=============================== protected =================================*/

/*================================ private ==================================*/
