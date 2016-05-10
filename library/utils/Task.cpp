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

Task::Task()
{
}

void Task::delay(uint32_t milliseconds)
{
    vTaskDelay(milliseconds / portTICK_RATE_MS);
}

/*=============================== protected =================================*/

/*================================ private ==================================*/
