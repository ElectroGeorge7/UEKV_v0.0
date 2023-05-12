
#ifndef UB_CHECK_H_
#define UB_CHECK_H_

#include "stm32f4xx_hal.h"

#include "result_check.h"

#define UB_MATRIX_ROW_NUM	6
#define UB_MATRIX_COL_NUM	8

extern uint8_t ubMatrix[UB_MATRIX_ROW_NUM];

HAL_StatusTypeDef ub_check_init(TestConfig_t config);
HAL_StatusTypeDef ub_check_synchro(uint16_t *resBitMatrix);
HAL_StatusTypeDef ub_check_aver_start(void);
void ub_check_aver_finish(uint16_t *resBitMatrix);

HAL_StatusTypeDef ub_check_sig_level_wait(uint8_t sigNum , uint8_t sigLev, uint16_t timeout);
HAL_StatusTypeDef ub_check_new_res_wait(uint8_t sigNum, uint8_t sigLev, uint16_t sigMinDur, uint16_t timeout);
HAL_StatusTypeDef ub_check_freq_adjust(void);

ResCheckMethod_t ub_check_method_get(void);
void ub_res_clear(void);

#endif
