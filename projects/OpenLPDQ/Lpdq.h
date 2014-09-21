/*
 * Copyright 2013 OpenMote Technologies, S.L.
 */

/**
 *
 * @file       Lpdq.h
 * @author     Pere Tuset-Peiro (peretuset@openmote.com)
 * @version    v0.1
 * @date       May, 2014
 * @brief
 * @ingroup
 *
 */
 
#ifndef LPDQ_H_
#define LPDQ_H_

#include "Radio.h"
#include "Callback.h"

class Lpdq;

typedef GenericCallback<Lpdq> LpdqCallback;

class LpdqFrame
{
};

class Lpdq
{
public:
    Lpdq(Radio& radio_);
    void init(void);
protected:
    void rxInit(void);
    void rxDone(void);
    void txInit(void);
    void txDone(void);
private:

private:
    Radio& radio;
    
    LpdqCallback rxInitCallback;
    LpdqCallback rxDoneCallback;
    LpdqCallback txInitCallback;
    LpdqCallback txDoneCallback;
};

#endif // LPDQ_H_
