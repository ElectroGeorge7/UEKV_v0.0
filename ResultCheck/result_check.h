/*
 * result_check.h
 *
 *  Created on: Oct 15, 2022
 *      Author: George
 */

#include "stm32f4xx_hal.h"

#ifndef RESULT_CHECK_H_
#define RESULT_CHECK_H_

#define RESULT_MATRIX_MAX_ROW_NUM	10
#define RESULT_MATRIX_MAX_COL_NUM	14

extern uint16_t resultMatrix[RESULT_MATRIX_MAX_ROW_NUM];

HAL_StatusTypeDef result_check_init(void);
HAL_StatusTypeDef result_check_clear(void);

#endif /* RESULT_CHECK_H_ */
