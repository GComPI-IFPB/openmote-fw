/*
 * Copyright 2013 OpenMote Technologies, S.L.
 */

/**
 *
 * @file       stm32l1xx_it.h
 * @author     Pere Tuset-Peiro (peretuset@openmote.com)
 * @version    v0.1
 * @date       May, 2014
 * @brief
 * @ingroup
 *
 */

#ifndef __STM32L1xx_IT_H
#define __STM32L1xx_IT_H

#ifdef __cplusplus
 extern "C" {
#endif 

/*================================ include ==================================*/

#include "stm32l1xx.h"

/*================================ define ===================================*/

/*================================ typedef ==================================*/

void MemManage_Handler(void);
void BusFault_Handler(void);
void UsageFault_Handler(void);
void SVC_Handler(void);
void DebugMon_Handler(void);
void PendSV_Handler(void);
void SysTick_Handler(void);

/*=============================== variables =================================*/

/*=============================== prototypes ================================*/

/*================================= public ==================================*/

/*================================ private ==================================*/

#ifdef __cplusplus
}
#endif

#endif /* __STM32L1xx_IT_H */
