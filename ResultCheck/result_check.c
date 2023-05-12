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
#include "ett_check.h"

uint16_t resultMatrix[RESULT_MATRIX_MAX_ROW_NUM];

static TestType_t testType = RELIABILITY_TEST;
static uint8_t confRowNum = 0;
static uint8_t confColNum = 0;
static uint8_t confCellNum = 0;
static uint8_t confEttPcbNum = 0;

extern ResCellStatus_t ledsMatrix[LEDS_MATRIX_ROW_NUM][LEDS_MATRIX_COL_NUM];

HAL_StatusTypeDef result_check_init(TestConfig_t conf){

	testType = conf.testType;
	confRowNum = conf.rowNum;
	confColNum = conf.colNum;
	confCellNum = conf.cellNum;
	confEttPcbNum = conf.pcbNum;

	if ( testType == RELIABILITY_TEST ){
		ub_check_init(conf);
	} else if ( testType == ETT_TEST ){
		ett_check_init(conf);
	};

	return HAL_OK;
}

HAL_StatusTypeDef result_show(uint16_t *resBitMatrix){
	uint8_t startRowNum = 0;
	uint8_t startColNum = 0;
	uint8_t cellNum = 0;

	leds_matrix_clear();
	HAL_Delay(50);

	if ( testType == RELIABILITY_TEST ){

		cellNum = 0;
		for (uint8_t rowNum = 0; rowNum < confRowNum; rowNum++){
			for(uint8_t colNum = 0; (colNum < confColNum) && (cellNum < confCellNum); colNum++){
				ledsMatrix[rowNum][colNum] = ( resBitMatrix[rowNum] & (1<<colNum) ) ? RES_CELL_OK : RES_CELL_FAULT;
				cellNum++;
			}
		}

	} else if ( testType == ETT_TEST ){

		for (uint8_t pcbNum = 0; pcbNum < confEttPcbNum; pcbNum++){
			switch(pcbNum){
			case 0:
				startRowNum = 0;
				startColNum = 0;
				break;
			case 1:
				startRowNum = RESULT_MATRIX_MAX_ROW_NUM / 2;
				startColNum = 0;
				break;
			case 2:
				startRowNum = 0;
				startColNum = RESULT_MATRIX_MAX_COL_NUM / 2;
				break;
			case 3:
				startRowNum = RESULT_MATRIX_MAX_ROW_NUM / 2;
				startColNum = RESULT_MATRIX_MAX_COL_NUM / 2;
				break;
			default:
				startRowNum = 0;
				startColNum = 0;
			}

			cellNum = 0;
			for (uint8_t rowNum = startRowNum; rowNum < (confRowNum+startRowNum); rowNum++){
				for(uint8_t colNum = startColNum; (colNum < (confColNum+startColNum)) && (cellNum < confCellNum); colNum++){
					ledsMatrix[rowNum][colNum] = ( resBitMatrix[rowNum] & (1<<colNum) ) ? RES_CELL_OK : RES_CELL_FAULT;
					cellNum++;
				}
			}
		}

	}

	leds_matrix_show_result();

	return HAL_OK;
}

HAL_StatusTypeDef result_check_deinit(void){
	rch_timer_stop();
	result_check_clear();
	leds_matrix_clear();
	return HAL_OK;
}

HAL_StatusTypeDef result_check_clear(void){
	if ( testType == RELIABILITY_TEST ){
		ub_res_clear();
	} else if ( testType == ETT_TEST ){
		ett_res_clear();
	};
	memset(resultMatrix, 0, sizeof(resultMatrix));
	return HAL_OK;
}
