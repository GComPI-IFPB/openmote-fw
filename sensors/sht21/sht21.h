/*
 * Copyright 2013 OpenMote Technologies, S.L.
 */

/**
 *
 * @file       sht21.h
 * @author     Pere Tuset-Peiro (peretuset@openmote.com)
 * @version    v0.1
 * @date       May, 2014
 * @brief
 * @ingroup
 *
 */

#ifndef SHT21_H_
#define SHT21_H_

class Sht21 {
public:
    Sht21();
    void reset(void);
    void readTemperature(void);
    void readHumidity(void)
    float getTemperature(void);
    float getHumidity(void);
private:
    uint16_t temperature;
    uint16_t humidity;
};

#endif /* SHT21_H_ */
