/*
 * result_check.h
 *
 *  Created on: Oct 15, 2022
 *      Author: George
 */

#include "stm32f4xx_hal.h"

#include "leds_matrix.h"
#include "rtc_hardware.h"

#ifndef RESULT_CHECK_H_
#define RESULT_CHECK_H_

#define RESULT_MATRIX_MAX_ROW_NUM	LEDS_MATRIX_ROW_NUM
#define RESULT_MATRIX_MAX_COL_NUM	LEDS_MATRIX_COL_NUM

extern uint16_t resultMatrix[RESULT_MATRIX_MAX_ROW_NUM];

typedef enum {
	RES_CELL_NO = 0,
	RES_CELL_OK,
	RES_CELL_FAULT
} ResCellStatus_t;

typedef enum TestType{
	RELIABILITY_TEST,
	ETT_TEST,
} TestType_t;

typedef enum ResCheckMethod{
	AVERAGE_RESULT,
	SYNCHRO_RESULT
} ResCheckMethod_t;

typedef enum ResCheckOption{
	NO_OPTION = 0,
	JUST_FAULTES,
	TRIAL_1DAY,
	TRIAL_UB_1WEEK
} ResCheckOption_t;

typedef struct {
	char partNumber[34];
	char mldrNum[10];
	TestType_t testType;
	uint8_t cellNum;
	uint8_t	rowNum;
	uint8_t colNum;
	ResCheckMethod_t resCheckMethod;
	uint8_t resCheckPeriod;				// in seconds, only for average method
//	ResCheckOption_t resCheckOption;
//	DataTime_t testStartDataTime;
//	uint32_t testDurationInHours;
//	uint8_t powerSupplyNum;
	uint8_t	pcbNum;						// only for ett
} TestConfig_t;

HAL_StatusTypeDef result_check_init(TestConfig_t conf);
HAL_StatusTypeDef result_show(uint16_t *resBitMatrix);
HAL_StatusTypeDef result_check_deinit(void);
HAL_StatusTypeDef result_check_clear(void);

#endif /* RESULT_CHECK_H_ */
