/*
 * Copyright 2013 OpenMote Technologies, S.L.
 */

/**
 *
 * @file       Mac.h
 * @author     Pere Tuset-Peiro (peretuset@openmote.com)
 * @version    v0.1
 * @date       May, 2014
 * @brief
 * @ingroup
 *
 */
 
#ifndef MAC_H_
#define MAC_H_

class Mac
{
public:
    Mac();
    transmitFrame();
    receiveFrame();
private:
    Radio& radio;
    
    uint8_t transmitBuffer[128];
    uint8_t* transmitPtr;
    uint32_t transmitLen;
    
    uint8_t receiveBuffer[128];
    uint8_t* receivePtr;
    uint32_t receiveLen;

    uint32_t receivedFrames;
    uint32_t receivedFramesError;
    uint32_t sentFrames;
    uint32_t sentFramesError;
};

#endif // MAC_H_
