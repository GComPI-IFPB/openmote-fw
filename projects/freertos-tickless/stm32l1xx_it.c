/*
 * Copyright 2013 OpenMote Technologies, S.L.
 */

/**
 *
 * @file       stm32l1xx_it.c
 * @author     Pere Tuset-Peiro (peretuset@openmote.com)
 * @version    v0.1
 * @date       May, 2014
 * @brief
 * @ingroup
 *
 */

/*================================ include ==================================*/

#include "stm32l1xx_it.h"
#include "stm32l1xx_exti.h"
#include "stm32l1xx_rtc.h"

/*================================ define ===================================*/

/*================================ typedef ==================================*/

/*=============================== variables =================================*/

/*=============================== prototypes ================================*/

/*================================= public ==================================*/

void RTC_WKUP_IRQHandler(void)
{
  RTC_ClearITPendingBit(RTC_IT_WUT);
  EXTI_ClearITPendingBit(EXTI_Line20);
}

/*================================ private ==================================*/

