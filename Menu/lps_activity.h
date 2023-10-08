/*
 * lps_activity.h
 *
 *  Created on: 23 февр. 2023 г.
 *      Author: George
 */

#ifndef LPS_ACTIVITY_H_
#define LPS_ACTIVITY_H_

#include "stm32f4xx_hal.h"
#include "activity.h"

HAL_StatusTypeDef lps_view_update(Command_t lpsAction, uint8_t *data);
void lps_reg_activity_cb(ActivityViewUpdateCb_t *pLpsUpdateCb);

#endif /* LPS_ACTIVITY_H_ */
