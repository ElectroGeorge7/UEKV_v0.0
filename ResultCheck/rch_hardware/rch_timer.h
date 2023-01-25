#ifndef RCH_HARDWARE_RCH_TIMER_H_
#define RCH_HARDWARE_RCH_TIMER_H_

#include "stm32f4xx_hal.h"

HAL_StatusTypeDef rch_timer_init(void);
HAL_StatusTypeDef rch_timer_start(void);
HAL_StatusTypeDef rch_timer_stop(void);

#endif
