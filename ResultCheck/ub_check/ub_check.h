
#ifndef UB_CHECK__
#define UB_CHECK__

#include "stm32f4xx_hal.h"

#define UB_MATRIX_ROW_NUM	8
#define UB_MATRIX_COL_NUM	8

uint8_t ubMatrix[8];

HAL_StatusTypeDef ub_check_init();
HAL_StatusTypeDef ub_check();
HAL_StatusTypeDef ub_res_clear(void);

#endif
