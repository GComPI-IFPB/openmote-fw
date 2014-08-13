/*
 * Copyright 2013 OpenMote Technologies, S.L.
 */

/**
 *
 * @file       Callback.h
 * @author     Pere Tuset-Peiro (peretuset@openmote.com)
 * @version    v0.1
 * @date       May, 2014
 * @brief
 * @ingroup
 *
 */

#ifndef CALLBACK_H_
#define CALLBACK_H_

class Callback
{
public:
    virtual void execute(void) = 0;
private:
};

#endif /* CALLBACK_H_ */
