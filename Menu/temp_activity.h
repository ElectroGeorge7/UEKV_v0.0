#ifndef TEMP_ACTIVITY_H_
#define TEMP_ACTIVITY_H_

#include "stm32f4xx_hal.h"
#include "activity.h"

HAL_StatusTypeDef temp_view_update(Command_t tempAction, uint8_t *data);
void temp_reg_activity_cb(ActivityViewUpdateCb_t *pTempUpdateCb);

#endif /* TEMP_ACTIVITY_H_ */
