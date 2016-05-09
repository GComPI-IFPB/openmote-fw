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

#ifndef SEMAPHORE_H_
#define SEMAPHORE_H_

#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "queue.h"

class Semaphore
{
public:
    Semaphore();
    ~Semaphore();
    bool take(void);
    bool take(uint32_t milliseconds);
    void give(void);
    void giveFromInterrupt(void);
protected:
    SemaphoreHandle_t semaphore_;
    BaseType_t priorityTaskWoken_;
};

class SemaphoreBinary : public Semaphore
{
public:
    SemaphoreBinary(bool given = true);
};

class SemaphoreCounting : public Semaphore
{
public:
    SemaphoreCounting(uint32_t initialCount, uint32_t maxCount);
};

#endif /* SEMAPHORE_H_ */
