/**
 * @file       Semaphore.h
 * @author     Pere Tuset-Peiro (peretuset@openmote.com)
 * @version    v0.1
 * @date       May, 2015
 * @brief
 *
 * @copyright  Copyright 2015, OpenMote Technologies, S.L.
 *             This file is licensed under the GNU General Public License v2.
 */

#ifndef RENDEZVOUZ_H_
#define RENDEZVOUZ_H_

#include "FreeRTOS.h"
#include "task.h"
#include "event_groups.h"

class Rendezvouz
{
public:
    Rendezvouz();
    ~Rendezvouz();
    uint8_t getTaskId(void);
    bool sync(uint8_t taskId);
    bool sync(uint8_t taskId, uint32_t milliseconds);
protected:
    EventGroupHandle_t groupHandle_;
    uint8_t taskCounter;
    uint8_t lastTaskId;
    uint8_t maxTaskId;
};

#endif /* RENDEZVOUZ_H_ */
