/**
 * @file       TemperatureSensor.h
 * @author     Pere Tuset-Peiro (peretuset@openmote.com)
 * @version    v0.1
 * @date       January, 2016
 * @brief
 *
 * @copyright  Copyright 2016, OpenMote Technologies, S.L.
 *             This file is licensed under the GNU General Public License v2.
 */

#ifndef TEMPERATURE_SENSOR_H_
#define TEMPERATURE_SENSOR_H_

#include <stdint.h>

#include "Sensor.h"

class TemperatureSensor : public Sensor
{
public:
    TemperatureSensor();
    bool enable(void);
    bool disable(void);
    bool suspend(void);
    bool wakeup(void);
    bool reset(void);
    bool isPresent(void);
    bool readTemperature(void);
    uint16_t getTemperatureRaw(void);
    float getTemperature(void);
private:
    uint16_t temperature;
};

#endif /* TEMPERATURE_SENSOR_H_ */

