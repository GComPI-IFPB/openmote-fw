/*
 * Copyright 2013 OpenMote Technologies, S.L.
 */

/**
 *
 * @file       SerialManager.h
 * @author     Pere Tuset-Peiro (peretuset@openmote.com)
 * @version    v0.1
 * @date       May, 2014
 * @brief
 * @ingroup
 *
 */

#ifndef SERIAL_MANAGER_H_
#define SERIAL_MANAGER_H_

#include <stdint.h>

#include "Serial.h"

class SerialManager
{
public:
    SerialManager(Serial& serial_);
    void init(void);
    void registerCallback(void);
private:
    Serial& serial;
};

#endif /* SERIAL_H_ */
