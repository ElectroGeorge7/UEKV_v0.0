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

typedef enum TestType{
	RELIABILITY_TEST,
	ETT_TEST,
} TestType_t;

typedef enum ResCheckMethod{
	EVERY_RESULT,
	AVERAGE_RESULT_PER_1S,
	AVERAGE_RESULT_PER_2S,
	AVERAGE_RESULT_PER_3S,
	AVERAGE_RESULT_PER_4S,
	AVERAGE_RESULT_PER_5S,
	JUST_FAULTES,
	TRIAL_UB_1DAY,
	TRIAL_UB_1WEEK,
	TRIAL_UB_4WEEK,
	TRIAL_ETT_1DAY,
	TRIAL_ETT_1WEEK,
	TRIAL_ETT_4WEEK
} ResCheckMethod_t;

HAL_StatusTypeDef result_check_init(TestType_t type, ResCheckMethod_t method);
HAL_StatusTypeDef result_check_deinit(void);
HAL_StatusTypeDef result_check_clear(void);

#endif /* RESULT_CHECK_H_ */
