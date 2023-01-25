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

HAL_StatusTypeDef test_view_update(Command_t TestAction, uint8_t *data);
void test_reg_activity_cb(ActivityViewUpdateCb_t *pTestUpdateCb);

#endif /* TEST_ACTIVITY_H_ */
