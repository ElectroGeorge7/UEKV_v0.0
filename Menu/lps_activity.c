/*
 * lps_activity.c
 *
 *  Created on: 23 февр. 2023 г.
 *      Author: George
 */

#include "lps_activity.h"

#include "cmsis_os2.h"

#include "terminal.h"
#include <stdio.h>
#include <string.h>

#include "LCD1602.h"
#include "rtc_hardware.h"

#include "lps_task.h"

extern osEventFlagsId_t testEvents;

#define LPS_ACT_MENU_ROW_NUM 2
static char lpsActMenu[LPS_ACT_MENU_ROW_NUM][32] = {"ИП всего: ", "Конф-ия ИП: "}; // cyrillic letters take 2 bytes
static uint8_t curCursorPos = 0;

#define LPS_ACT_MENU_START     	0x01
#define LPS_ACT_FIND_CONNECTED	0x02
#define LPS_ACT_CONFIG_ALL		0x04
static uint8_t lpsActStatusFlags = 0;

char rs485Buf[54] = {0};

HAL_StatusTypeDef lps_view_update(Command_t lpsAction, uint8_t *data){

	char lcdStr[32] = {0};
	uint32_t osEventFlag = 0;

	switch (lpsAction){
		case START_CMD:
			break;
		case SELECT_CMD:

            if ( !lpsActStatusFlags ){
                LCD_Clear();
                LCD_SetCursor( 0, 0 );
                LCD_PrintString(lpsActMenu[0]);
                LCD_SetCursor( 0, 1 );
                LCD_PrintString(lpsActMenu[1]);
                LCD_SetCursor( 15, curCursorPos = 0 );

                lpsActStatusFlags = LPS_ACT_MENU_START;
            } else if ( (curCursorPos == 0) && (lpsActStatusFlags != LPS_ACT_FIND_CONNECTED) ){

                LCD_SetCursor( 0, 0 );
                LCD_PrintString("                ");
                LCD_SetCursor( 0, 0 );
                LCD_PrintString(lpsActMenu[0]);
                LCD_SetCursor( 15, curCursorPos = 0 );

                osEventFlagsSet(testEvents, LPS_FIND_CONNECTED_START);
                if ( (osEventFlag = osEventFlagsWait(testEvents, LPS_FIND_CONNECTED_FINISHED, osFlagsWaitAny, osWaitForever)) & LPS_FIND_CONNECTED_FINISHED ){
					//lps_find_connected();
					uint8_t lpsNum = lps_get_connected_num();

					LCD_SetCursor( 0, 0 );
					memset(lcdStr, 0, sizeof(lcdStr));
					snprintf(lcdStr, 32, "%s%d", lpsActMenu[0], lpsNum);
					LCD_PrintString(lcdStr);
					LCD_SetCursor( 15, curCursorPos = 0 );
                }

                lpsActStatusFlags = LPS_ACT_FIND_CONNECTED;
            } else if ( (curCursorPos == 1) && (lpsActStatusFlags != LPS_ACT_CONFIG_ALL) ){

                LCD_SetCursor( 0, 1 );
                LCD_PrintString("                ");
                LCD_SetCursor( 0, 1 );
                LCD_PrintString(lpsActMenu[1]);
                LCD_SetCursor( 15, curCursorPos = 1 );

                osEventFlagsSet(testEvents, LPS_CONFIG_SEARCH);
                osEventFlag = osEventFlagsWait(testEvents, LPS_CONFIG_FINISH, osFlagsWaitAny, osWaitForever);

                if ( (osEventFlag & LPS_CONFIG_FINISH) && (osEventFlag & LPS_CONFIG_ERROR) ){
                	osEventFlagsClear(testEvents, LPS_CONFIG_ERROR);

					LCD_SetCursor( 0, 1 );
					memset(lcdStr, 0, sizeof(lcdStr));
					snprintf(lcdStr, 32, "%s%s", lpsActMenu[1], "ERR");
					LCD_PrintString(lcdStr);
					LCD_SetCursor( 15, curCursorPos = 1 );
                } else if ( osEventFlag  & LPS_CONFIG_FINISH ){
					LCD_SetCursor( 0, 1 );
					memset(lcdStr, 0, sizeof(lcdStr));
					snprintf(lcdStr, 32, "%s%s", lpsActMenu[1], "OK");
					LCD_PrintString(lcdStr);
					LCD_SetCursor( 15, curCursorPos = 1 );
                }

            	lpsActStatusFlags = LPS_ACT_CONFIG_ALL;
            }
			break;
        case UP_CMD:
        	LCD_SetCursor( 15, curCursorPos = 0 );
            break;
        case DOWN_CMD:
        	LCD_SetCursor( 15, curCursorPos = 1 );
            break;
		case BACK_CMD:
			activity_change(MENU_ACTIVITY);
			lpsActStatusFlags = curCursorPos = 0;
			break;
		case TERMINAL_CMD:
			break;
	  default:
			break;
	}

	return HAL_OK;
}


void lps_reg_activity_cb(ActivityViewUpdateCb_t *pLpsUpdateCb){
	*pLpsUpdateCb = lps_view_update;
}
