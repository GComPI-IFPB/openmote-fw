/*
 * Copyright 2013 OpenMote Technologies, S.L.
 */

/**
 *
 * @file       cc2538_it.h
 * @author     Pere Tuset-Peiro (peretuset@openmote.com)
 * @version    v0.1
 * @date       May, 2014
 * @brief
 * @ingroup
 *
 */

#ifndef __CC2538_IT_H
#define __CC2538_IT_H

#ifdef __cplusplus
 extern "C" {
#endif 

/*================================ include ==================================*/

/*================================ define ===================================*/

/*================================ typedef ==================================*/

/*=============================== variables =================================*/

/*=============================== prototypes ================================*/

void SVC_Handler(void);
void PendSV_Handler(void);
void SysTick_Handler(void);

/*================================= public ==================================*/

/*================================ private ==================================*/

#ifdef __cplusplus
}
#endif

#endif /* __CC2538_IT_H */
