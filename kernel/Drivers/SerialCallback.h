/*
 * Copyright 2013 OpenMote Technologies, S.L.
 */

/**
 *
 * @file       SerialCallback.h
 * @author     Pere Tuset-Peiro (peretuset@openmote.com)
 * @version    v0.1
 * @date       May, 2014
 * @brief
 * @ingroup
 *
 */

#ifndef SERIAL_CALLBACK_H_
#define SERIAL_CALLBACK_H_

#include "Callback.h"

class Serial;

class SerialCallback : public Callback
{
public:
    SerialCallback(Serial* object_ = nullptr, \
                   void(Serial:: *method_)(void) = nullptr ):
                   object(object_), method(method_){}
    void execute(void) {(object->*method)();}
private:
    Serial* object;
    void(Serial:: *method)(void);
};

#endif /* SERIAL_CALLBACK_H_ */
