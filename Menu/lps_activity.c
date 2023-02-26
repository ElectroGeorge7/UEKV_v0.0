/*
 * lps_activity.c
 *
 *  Created on: 23 февр. 2023 г.
 *      Author: EKF
 */

#include "lps_activity.h"

#include "terminal.h"
#include <stdio.h>

#include "LCD1602.h"
#include "rtc_hardware.h"

#include "rs485_hardware.h"

/*
всего у этой активности должно быть 2 функции:
найти все доступные лбп и поуправлять выходом 
сконфигурировать лбп по config_lbp.txt
*/

#define LPS_ACT_MENU_ROW_NUM 2
static char lpsActMenu[LPS_ACT_MENU_ROW_NUM][32] = {"ЛБП всего: ", "Конф-ия ЛБП: "}; // cyrillic letters take 2 bytes
static uint8_t curCursorPos = 0;

#define LPS_ACT_TMENU_START     0x01
static uint8_t lpsActStatusFlags = 0;

uint8_t ADR01_cmd[] = ":ADR01;\r";
uint8_t MDL_cmd[] = ":MDL?;\r";
uint8_t REV_cmd[] = ":REV?;\r";
uint8_t RMT0_cmd[] = ":RMT0;\r";
uint8_t RMT1_cmd[] = ":RMT1;\r";
uint8_t RMT2_cmd[] = ":RMT2;\r";
uint8_t OUT1_cmd[] = ":OUT1;\r";
uint8_t OUT0_cmd[] = ":OUT0;\r";
uint8_t STT_cmd[] = ":STT?;\r";
char rs485Buf[54] = {0};

HAL_StatusTypeDef lps_view_update(Command_t lpsAction, uint8_t *data){


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


                rs485_init();
                HAL_Delay(100);

                rs485_transmit(ADR01_cmd, sizeof(ADR01_cmd));
                rs485_transmit(OUT1_cmd, sizeof(OUT1_cmd));
                HAL_Delay(5000);
                rs485_transmit(OUT0_cmd, sizeof(OUT1_cmd));

                rs485_transmit_w_respond(STT_cmd, sizeof(STT_cmd), rs485Buf, sizeof(rs485Buf));



                lpsActStatusFlags = LPS_ACT_TMENU_START;
            } else if ( curCursorPos == 0 ){
                uint8_t lpsNum = 0;
                // определить количество ЛБП в сети
                //lpsNum = lps_detect();
                // кратковременно переключить выход каждого ЛБП

                snprintf(lpsActMenu[0] + sizeof(lpsActMenu[0]), 32, "%2d", lpsNum);

                LCD_SetCursor( 0, 0 );
                LCD_PrintString(lpsActMenu[0]);
                LCD_SetCursor( 15, curCursorPos );

            } else if ( curCursorPos == 1 ){
                // найти config_lbp.txt файл
                // и установить ограничения для ЛБП в соответствии с этим файлом
            }


			break;
        case UP_CMD:
            break;
        case DOWN_CMD:
            break;
		case BACK_CMD:
			activity_change(MENU_ACTIVITY);
			lpsActStatusFlags = 0;
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
