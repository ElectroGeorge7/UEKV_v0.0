
#ifndef LM_TIMER_H
#define LM_TIMER_H

#include "stm32f4xx_hal.h"

HAL_StatusTypeDef lm_timer_init(void);
HAL_StatusTypeDef lm_timer_start(uint32_t *buf, uint32_t size);
HAL_StatusTypeDef lm_timer_stop(void);

#endif
