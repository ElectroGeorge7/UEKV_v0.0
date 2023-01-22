#ifndef DATE_TIME_ACTIVITY_H__
#define DATE_TIME_ACTIVITY_H__

#include "stm32f4xx_hal.h"
#include "activity.h"

HAL_StatusTypeDef date_time_view_update(Command_t dateTimeAction, uint8_t *data);
void date_time_reg_activity_cb(ActivityViewUpdateCb_t *pDateTimeUpdateCb);

#endif // DATE_TIME_ACTIVITY_H__

