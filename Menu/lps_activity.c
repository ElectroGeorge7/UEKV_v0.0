/*
 * lps_activity.c
 *
 *  Created on: 23 февр. 2023 г.
 *      Author: EKF
 */

#include "lps_activity.h"

#include "terminal.h"
#include <stdio.h>
#include <string.h>

#include "LCD1602.h"
#include "rtc_hardware.h"

#include "lps_control.h"

/*
всего у этой активности должно быть 2 функции:
найти все доступные лбп и поуправлять выходом 
сконфигурировать лбп по config_lbp.txt
*/

#define LPS_ACT_MENU_ROW_NUM 2
static char lpsActMenu[LPS_ACT_MENU_ROW_NUM][32] = {"ЛБП всего: ", "Конф-ия ЛБП: "}; // cyrillic letters take 2 bytes
static uint8_t curCursorPos = 0;

#define LPS_ACT_MENU_START     	0x01
#define LPS_ACT_FIND_CONNECTED	0x02
#define LPS_ACT_CONFIG_ALL		0x04
static uint8_t lpsActStatusFlags = 0;

char rs485Buf[54] = {0};

HAL_StatusTypeDef lps_view_update(Command_t lpsAction, uint8_t *data){

	char lcdStr[32] = {0};

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

                //rs485_init();
                //HAL_Delay(100);

                lpsActStatusFlags = LPS_ACT_MENU_START;
            } else if ( (curCursorPos == 0) && (lpsActStatusFlags != LPS_ACT_FIND_CONNECTED) ){
                // определить количество ЛБП в сети
                //lpsNum = lps_detect();
                // кратковременно переключить выход каждого ЛБП

                LCD_Clear();
                LCD_SetCursor( 0, 0 );
                LCD_PrintString(lpsActMenu[0]);
                LCD_SetCursor( 0, 1 );
                LCD_PrintString(lpsActMenu[1]);
                LCD_SetCursor( 15, curCursorPos = 0 );

                lps_find_connected();
                uint8_t lpsNum = lps_get_connected_quantity();

                LCD_SetCursor( 0, 0 );
                snprintf(lcdStr, 32, "%s%d", lpsActMenu[0], lpsNum);
                LCD_PrintString(lcdStr);
                LCD_SetCursor( 15, curCursorPos = 0 );

                //memset(rs485Buf, 0, sizeof(rs485Buf) );
                //lps_read_status(1, rs485Buf, sizeof(rs485Buf));

                lpsActStatusFlags = LPS_ACT_FIND_CONNECTED;
            } else if ( (curCursorPos == 1) && (lpsActStatusFlags != LPS_ACT_CONFIG_ALL) ){
                // найти config_lbp.txt файл
                // и установить ограничения для ЛБП в соответствии с этим файлом
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
