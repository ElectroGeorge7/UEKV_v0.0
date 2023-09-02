/*
 * temp_activity.c
 *
 *  Created on: 2 сент. 2023 г.
 *      Author: George
 */

#include "temp_activity.h"

#include <stdio.h>

#include "ts_spi.h"
#include "LCD1602.h"

HAL_StatusTypeDef temp_view_update(Command_t tempAction, uint8_t *data){
	uint8_t i = 0;
	char pBufRow1[32] = {0};
	char pBufRow2[32] = {0};
	float temp1 = 0.0;
	float temp2 = 0.0;

	switch (tempAction){
		case START_CMD:
			break;
		case SELECT_CMD:

			temp1 = ts_check(1);
			temp2 = ts_check(2);

			snprintf(pBufRow1, 32, "темп.1:%3.0fC", temp1);
			snprintf(pBufRow2, 32, "темп.2:%3.0fC", temp2);

			LCD_Clear();
			LCD_CursorOnOff(0);
			LCD_SetCursor( 0, 0 );
			LCD_PrintString(pBufRow1);
			LCD_SetCursor( 0, 1 );
			LCD_PrintString(pBufRow2);

			break;
		case BACK_CMD:
			activity_change(MENU_ACTIVITY);
			break;
		case TERMINAL_CMD:
			break;
	  default:
			break;
	}

	return HAL_OK;
}


void temp_reg_activity_cb(ActivityViewUpdateCb_t *pTempUpdateCb){
	*pTempUpdateCb = temp_view_update;
}
