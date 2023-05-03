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

#include "result_check.h"

HAL_StatusTypeDef test_view_update(Command_t TestAction, uint8_t *data);
HAL_StatusTypeDef test_terminal_config(uint8_t *data, TestConfig_t *curConfig, uint16_t *flag);
void test_reg_activity_cb(ActivityViewUpdateCb_t *pTestUpdateCb);

#endif /* TEST_ACTIVITY_H_ */
