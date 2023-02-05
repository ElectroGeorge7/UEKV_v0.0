/*
 * reliability_test_activity.h
 *
 *  Created on: 25 янв. 2023 г.
 *      Author: Giorgi
 */

#ifndef TEST_ACTIVITY_H_
#define TEST_ACTIVITY_H_

#include "stm32f4xx_hal.h"
#include "activity.h"

#include "rtc_hardware.h"

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

typedef struct {
	char partNumber[34];
	char mldrNum[10];
	TestType_t testType;
	//uint32_t curResNum;		// number of current displayed result
	uint8_t cellNum;
	uint8_t	rowNum;
	uint8_t colNum;
	ResCheckMethod_t resCheckMethod;
	DataTime_t testStartDataTime;
	uint32_t testDurationInHours;
	uint8_t powerSupplyNum;
	uint8_t	pcbNum;		// only for ett
} TestConfig_t;

HAL_StatusTypeDef test_view_update(Command_t TestAction, uint8_t *data);
HAL_StatusTypeDef test_terminal_config(uint8_t *data, TestConfig_t *curConfig, uint16_t *flag);
void test_reg_activity_cb(ActivityViewUpdateCb_t *pTestUpdateCb);

#endif /* TEST_ACTIVITY_H_ */
