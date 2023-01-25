/*
 * reliability_test_activity.c
 *
 *  Created on: 25 янв. 2023 г.
 *      Author: Giorgi
 */

#include "test_activity.h"

#include "terminal.h"
#include <stdio.h>

#include "LCD1602.h"
#include "rtc_hardware.h"

static uint8_t testStatus = 0;
static uint8_t curLcdRow = 0;

HAL_StatusTypeDef test_view_update(Command_t testAction, uint8_t *data){
	char pBuf[16] = {0};
	DataTime_t dataTime;

	int hour = 0;
	int min = 0;
	int day = 0;
	int mon = 0;
	int year = 0;

	switch (testAction){
		case START_CMD:
			;
			break;
        case UP_CMD:
            break;
        case DOWN_CMD:
            break;
		case SELECT_CMD:

			// check config file
			// wait until usb config
			// after config info was received display it and start test();
			// result_check_init()

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

void test_reg_activity_cb(ActivityViewUpdateCb_t *pTestUpdateCb){
    *pTestUpdateCb = test_view_update;
}
