/*
 * result_check.c
 *
 *  Created on: Oct 15, 2022
 *      Author: George
 */

#include "result_check.h"

#include <string.h>

#include "terminal.h"
#include "rch_timer.h"
#include "ub_check.h"

uint16_t resultMatrix[RESULT_MATRIX_MAX_ROW_NUM];

HAL_StatusTypeDef result_check_init(void){

	/* UB check*/
	ub_check_init();
	rch_timer_init();
	// phase alignment on the first signal of row1
	uartprintf("phase alignment on the first signal of row1: wait");
	rch_timer_start();
//	if ( ub_check_sig_level_wait(0, 1, 0xffff) == HAL_OK){
//		rch_timer_start();
//		uartprintf("phase alignment on the first signal of row1: ok");
//		return HAL_OK;
//	}

	return HAL_ERROR;
}

HAL_StatusTypeDef result_check_deinit(void){
	rch_timer_stop();
	result_check_clear();
	leds_matrix_clear();
	return HAL_OK;
}

HAL_StatusTypeDef result_check_clear(void){
	ub_res_clear();
	memset(resultMatrix, 0, sizeof(resultMatrix));
	return HAL_OK;
}
