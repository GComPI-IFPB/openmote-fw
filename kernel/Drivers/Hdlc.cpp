/*
 * Copyright 2013 OpenMote Technologies, S.L.
 */

/**
 *
 * @file       Hdlc.cpp
 * @author     Pere Tuset-Peiro (peretuset@openmote.com)
 * @version    v0.1
 * @date       May, 2014
 * @brief
 * @ingroup
 *
 */

/*================================ include ==================================*/

#include "Hdlc.h"

/*================================ define ===================================*/

#define HDLC_HEADER_SIZE            ( 4 ) // Flag + Command + Address
#define HDLC_FOOTER_SIZE            ( 3 ) // CRC + Flag

#define HDLC_FLAG                   ( 0x7E )
#define HDLC_ESCAPE                 ( 0x7D )
#define HDLC_ESCAPE_MASK            ( 0x20 )

/*================================ typedef ==================================*/

/*=============================== variables =================================*/

/*=============================== prototypes ================================*/

/*================================= public ==================================*/

Hdlc::Hdlc()
{
}

/*=============================== protected =================================*/

/*================================ private ==================================*/
