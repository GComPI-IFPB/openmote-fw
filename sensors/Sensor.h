/*
 * Copyright 2013 OpenMote Technologies, S.L.
 */

/**
 *
 * @file       Sensor.h
 * @author     Pere Tuset-Peiro (peretuset@openmote.com)
 * @version    v0.1
 * @date       May, 2014
 * @brief
 * @ingroup
 *
 */

#ifndef SENSOR_H_
#define SENSOR_H_

class Sensor {
public:
    virtual bool enable(void) = 0;
    virtual bool reset(void) = 0; 
    virtual bool isPresent(void) = 0;
private:
};

#endif /* SENSOR_H_ */
