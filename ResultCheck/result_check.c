/*
 * result_check.c
 *
 *  Created on: Oct 15, 2022
 *      Author: George
 */

#include "result_check.h"

#include <string.h>

#include "rch_timer.h"
#include "ub_check.h"

uint16_t resultMatrix[RESULT_MATRIX_MAX_ROW_NUM];

HAL_StatusTypeDef result_check_init(void){
	ub_check_init();
	rch_timer_init();
	return HAL_OK;
}

HAL_StatusTypeDef result_check_clear(void){
	ub_res_clear();
	memset(resultMatrix, 0, sizeof(resultMatrix));
	return HAL_OK;
}
