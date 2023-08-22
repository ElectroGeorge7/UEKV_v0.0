/*
 * date_time_activity.c
 *
 *  Created on: 22 янв. 2023 г.
 *      Author: George
 */

#include "date_time_activity.h"

#include "terminal.h"
#include <stdio.h>

#include "LCD1602.h"
#include "rtc_hardware.h"


HAL_StatusTypeDef date_time_view_update(Command_t dateTimeAction, uint8_t *data){
	uint8_t i = 0;
	char pBuf[16] = {0};
	DataTime_t dataTime;

	int hour = 0;
	int min = 0;
	int day = 0;
	int mon = 0;
	int year = 0;

	switch (dateTimeAction){
		case START_CMD:
			//date_time_activity_init();
			break;
		case SELECT_CMD:
			rtc_get(&dataTime);
			snprintf(pBuf, sizeof(pBuf), "%d:%d %d.%d.%d", dataTime.hour, dataTime.min, dataTime.day, dataTime.mon, dataTime.year);

			LCD_Clear();
			LCD_CursorOnOff(0);
			LCD_SetCursor( 0, 0 );
			LCD_PrintString(pBuf);
			LCD_SetCursor( 0, 1 );
			LCD_PrintString("Настр-ка по USB");

			usbprintf("Date and time set. Enter new date and time in format: hh:mm dd.mm.yyyy.");
			usbprintf("current date and time: %s", pBuf);
			break;
		case BACK_CMD:
			activity_change(MENU_ACTIVITY);
			break;
		case TERMINAL_CMD:
			sscanf(data, "%2d:%2d %2d.%2d.%4d", &hour, &min, &day, &mon, &year);
			usbprintf("new date and time: %d:%d %d.%d.%d", hour, min, day, mon, year);
			/// @todo add new time`s values check before rtc_set 
			dataTime.hour = hour; dataTime.min = min; dataTime.day = day;
			dataTime.mon = mon; dataTime.year = year;
			rtc_set(&dataTime);

			snprintf(pBuf, sizeof(pBuf), "%d:%d %d.%d.%d", dataTime.hour, dataTime.min, dataTime.day, dataTime.mon, dataTime.year);

			LCD_Clear();
			LCD_CursorOnOff(0);
			LCD_SetCursor( 0, 0 );
			LCD_PrintString(pBuf);
			LCD_SetCursor( 0, 1 );
			LCD_PrintString("Настр-ка по USB");

			break;
	  default:
			break;
	}

	return HAL_OK;
}


void date_time_reg_activity_cb(ActivityViewUpdateCb_t *pDateTimeUpdateCb){
	*pDateTimeUpdateCb = date_time_view_update;
}
