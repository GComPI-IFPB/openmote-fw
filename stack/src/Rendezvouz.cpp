/**
 * @file       Rendezvouz.cpp
 * @author     Pere Tuset-Peiro (peretuset@openmote.com)
 * @version    v0.1
 * @date       May, 2015
 * @brief
 *
 * @copyright  Copyright 2015, OpenMote Technologies, S.L.
 *             This file is licensed under the GNU General Public License v2.
 */

/*================================ include ==================================*/

#include "Rendezvouz.h"

/*================================ define ===================================*/

/*================================ typedef ==================================*/

/*=============================== variables =================================*/

/*=============================== prototypes ================================*/

/*================================= public ==================================*/

Rendezvouz::Rendezvouz():
	taskCounter(0), lastTaskId(0), maxTaskId(0)
{
	groupHandle_ = xEventGroupCreate();
    if (groupHandle_ == NULL) {
    	while(true);
    }
}

Rendezvouz::~Rendezvouz(void)
{
    vEventGroupDelete(groupHandle_);
}

uint8_t Rendezvouz::getTaskId(void)
{
	lastTaskId = 1 << (taskCounter);
	taskCounter++;
	maxTaskId |= lastTaskId;
	return lastTaskId;
}

bool Rendezvouz::sync(uint8_t taskId)
{
    xEventGroupSync(groupHandle_, taskId, maxTaskId, portMAX_DELAY);
    return true;
}

bool Rendezvouz::sync(uint8_t taskId, uint32_t milliseconds)
{
	EventBits_t uxReturn;

	TickType_t timeout = milliseconds / portTICK_RATE_MS;
	uxReturn = xEventGroupSync(groupHandle_,
                               taskId,
                               maxTaskId,
                               timeout);

	/* All three tasks reached the synchronisation point before the call
       to xEventGroupSync() timed out. */
    if ((uxReturn & maxTaskId) == maxTaskId) {
        return true;
    }

    return false;
}

/*=============================== protected =================================*/

/*================================ private ==================================*/
