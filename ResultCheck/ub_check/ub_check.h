
#ifndef UB_CHECK_H_
#define UB_CHECK_H_

#include "stm32f4xx_hal.h"

#define UB_MATRIX_ROW_NUM	8
#define UB_MATRIX_COL_NUM	8

extern uint8_t ubMatrix[8];

HAL_StatusTypeDef ub_check_init();
HAL_StatusTypeDef ub_check_sig_level_wait(uint8_t sigNum , uint8_t sigLev, uint16_t timeout);
HAL_StatusTypeDef ub_check_new_res_wait(uint8_t sigNum, uint8_t sigLev, uint16_t sigMinDur, uint16_t timeout);
HAL_StatusTypeDef ub_check_freq_adjust(void);
HAL_StatusTypeDef ub_check();
HAL_StatusTypeDef ub_res_clear(void);

#endif
