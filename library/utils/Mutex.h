/**
 * @file       Mutex.h
 * @author     Pere Tuset-Peiro (peretuset@openmote.com)
 * @version    v0.1
 * @date       May, 2015
 * @brief
 *
 * @copyright  Copyright 2015, OpenMote Technologies, S.L.
 *             This file is licensed under the GNU General Public License v2.
 */

#ifndef MUTEX_H_
#define MUTEX_H_

#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"

class Mutex
{
public:
    Mutex();
    ~Mutex();
    bool take(void);
    bool take(uint32_t milliseconds);
    void give(void);
private:
    SemaphoreHandle_t mutex_;
};

class MutexRecursive
{
public:
    MutexRecursive();
    ~MutexRecursive();
    bool take(void);
    bool take(uint32_t milliseconds);
    void give(void);
private:
    SemaphoreHandle_t mutex_;
};

#endif /* MUTEX_H_ */
