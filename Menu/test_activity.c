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

#define TEST_ACT_TEST_IS_ACTIVE 1
#define TEST_ACT_CONFIG_IS_SET 1
static uint8_t testActStatusFlags = 0;


static uint8_t curLcdRow = 0;

typedef enum TestType{
	RELIABILITY_TEST,
	ETT_TEST,
} TestType_t;

typedef enum ResCheckMethod{
	EVERY_RESULT,
	AVERAGE_RESULT_PER_1S,
	AVERAGE_RESULT_PER_2S,
	AVERAGE_RESULT_PER_3S,
	AVERAGE_RESULT_PER_4S,
	AVERAGE_RESULT_PER_5S,
	JUST_FAULTES,
	TRIAL_UB_1DAY,
	TRIAL_UB_1WEEK,
	TRIAL_UB_4WEEK,
	TRIAL_ETT_1DAY,
	TRIAL_ETT_1WEEK,
	TRIAL_ETT_4WEEK
} ResCheckMethod_t;

typedef struct {
	TestType_t testType;
	char *partNumber[16];
	char *mldrNum[4];
	//uint32_t curResNum;		// number of current displayed result
	uint8_t cellNum;
	uint8_t	rowNum;
	uint8_t colNum;
	ResCheckMethod_t resCheckMethod;
	DataTime_t testStartDataTime;
	uint32_t testDurationInHours;
	uint8_t powerSupplyNum;
	uint8_t	pcbNum;		// only for ett
} TestConfig_t;

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

			if ( !(testActStatusFlags & TEST_ACT_TEST_IS_ACTIVE) ){
				if ( !(testActStatusFlags & TEST_ACT_CONFIG_IS_SET) ){

					LCD_Clear();
					LCD_CursorOnOff(0);
					LCD_SetCursor( 0, 0 );
					LCD_PrintString("Поиск config.txt");
					LCD_SetCursor( 0, 1 );
					LCD_PrintString("...");
					HAL_Delay(1000);

					if (0/* check config file in file system */){
						// get configuration from the file
						testActStatusFlags |= TEST_ACT_CONFIG_IS_SET;
					} else {
						LCD_SetCursor( 0, 1 );
						LCD_PrintString("не найдено");
						HAL_Delay(1000);


						// enter config by usb cdc
						testActStatusFlags |= TEST_ACT_CONFIG_IS_SET;
					}
				}
			}

			// wait until usb config
			// after config info was received display it and start test();
			//result_check_init();

			break;
		case BACK_CMD:
			result_check_deinit();
			testActStatusFlags = 0;
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
