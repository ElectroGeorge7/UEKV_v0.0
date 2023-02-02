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

#define TEST_ACT_MENU_ROW_NUM 22
static uint8_t curMenuRow = 0;
static char testActMenu[TEST_ACT_MENU_ROW_NUM][32] = {0}; // cyrillic letters take 2 bytes

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
	char partNumber[32];
	char mldrNum[8];
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
	DataTime_t dataTime = {0};

	TestConfig_t curConfig = {
		RELIABILITY_TEST,
		"qwerty1234",
		"MLDR123",
		12,
		4,
		3,
		EVERY_RESULT,
		{0, 30, 14, 12, 2, 2023 },
		10000,
		1,
		1
	};

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
			if ( curMenuRow != 0 ){
				curMenuRow--;
				if ( curMenuRow%2 ){
					LCD_Clear();
					LCD_SetCursor( 0, 0 );
					LCD_PrintString(testActMenu[curMenuRow-1]);
					LCD_SetCursor( 0, 1 );
					LCD_PrintString(testActMenu[curMenuRow]);
					LCD_SetCursor( 15, 1 );
				}else{
					LCD_SetCursor( 15, 0 );
				}
			}
            break;
        case DOWN_CMD:
        	if ( (curMenuRow+1) < TEST_ACT_MENU_ROW_NUM){
        		curMenuRow++;
				if ( curMenuRow%2 ){
					LCD_SetCursor( 15, 1 );
				}else{
					LCD_Clear();
					LCD_SetCursor( 0, 0 );
					LCD_PrintString(testActMenu[curMenuRow]);
					LCD_SetCursor( 0, 1 );
					LCD_PrintString(testActMenu[curMenuRow+1]);
					LCD_SetCursor( 15, 0 );
				}
        	}
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


#ifdef TEST_ACT
					LCD_PrintString(curConfig.partNumber);
					LCD_PrintString("Рез.№:"); LCD_PrintString('0');
					LCD_PrintString(curConfig.mldrNum); LCD_PrintString(curConfig.cellNum); LCD_PrintString("шт.");

					LCD_PrintString("row:"); LCD_PrintString(curConfig.rowNum); LCD_PrintString("col:"); LCD_PrintString(curConfig.colNum);
					LCD_PrintString("Отобр-ие рез-та:");
					LCD_PrintString(curConfig.resCheckMethod);
					LCD_PrintString("тек. время:");
					LCD_PrintString(curDateTime);
					LCD_PrintString("начало испыт-ия:");
					LCD_PrintString(curConfig.testStartDataTime);
					LCD_PrintString("конец испыт-ия:");
					LCD_PrintString(curConfig.testStartDataTime + curConfig.testDurationInHours);
					LCD_PrintString("осталось:");
					LCD_PrintString(curConfig.testStartDataTime + curConfig.testDurationInHours - curDateTime);

					LCD_PrintString("кол-во Vпит:"); LCD_PrintString(curConfig.powerSupplyNum);
					LCD_PrintString("темп.№1:"); LCD_PrintString(temp1); LCD_PrintString('C');
					LCD_PrintString("темп.№2:"); LCD_PrintString(temp2); LCD_PrintString('C');
					LCD_PrintString("кол-во плат:"); LCD_PrintString(curConfig.pcbNum);
#endif
					if (0/* check config file in file system */){
						// get configuration from the file
						testActStatusFlags |= TEST_ACT_CONFIG_IS_SET;
					} else {
						LCD_SetCursor( 0, 1 );
						LCD_PrintString("не найдено");
						HAL_Delay(1000);

						snprintf(testActMenu[0], 32, "%s", curConfig.partNumber);
						snprintf(testActMenu[1], 32, "Рез.:0");

						snprintf(testActMenu[2], 32, "%s  %dшт.", curConfig.mldrNum, curConfig.cellNum);
						snprintf(testActMenu[3], 32, "row:%d    col:%d", curConfig.rowNum, curConfig.colNum);

						snprintf(testActMenu[4], 32, "Испитание:");
						snprintf(testActMenu[5], 32, "%s", curConfig.testType ? "ЭТТ" : "Безотказность");

						snprintf(testActMenu[6], 32, "Считыв. рез-та:");
						snprintf(testActMenu[7], 32, "%d", curConfig.resCheckMethod);

						snprintf(testActMenu[8], 32, "тек. время:");
						rtc_get(&dataTime);
						snprintf(testActMenu[9], 32, "%d:%d %d.%d.%d", dataTime.hour, dataTime.min, dataTime.day, dataTime.mon, dataTime.year);

						snprintf(testActMenu[10], 32, "начало испыт-ия:");
						memcpy(&(curConfig.testStartDataTime), &dataTime, sizeof(DataTime_t));
						snprintf(testActMenu[11], 32, "%d:%d %d.%d.%d", dataTime.hour, dataTime.min, dataTime.day, dataTime.mon, dataTime.year);

						snprintf(testActMenu[12], 32, "конец испыт-ия:");
						snprintf(testActMenu[13], 32, "%d:%d %d.%d.%d", 5, 0, 7, 5, 2023);

						snprintf(testActMenu[14], 32, "осталось:");
						snprintf(testActMenu[15], 32, "%d:%d %d.%d.%d", 5, 3, 2, 3, 0);

						snprintf(testActMenu[16], 32, "кол-во Vпит:%d", curConfig.powerSupplyNum);
						memset(testActMenu[17], 0, 32);

						float temp1 = Max6675_Read_Temp();
						float temp2 = Max6675_Read_Temp();
						snprintf(testActMenu[18], 32, "темп.№1:%3.0fC", temp1);
						snprintf(testActMenu[19], 32, "темп.№2:%3.0fC", temp2);

						snprintf(testActMenu[20], 32, "кол-во плат:%d", curConfig.pcbNum);
						memset(testActMenu[21], 0, 32);

						LCD_Clear();
						curMenuRow = 0;

						LCD_SetCursor( 0, 0 );
						LCD_PrintString(testActMenu[0]);
						LCD_SetCursor( 0, 1 );
						LCD_PrintString(testActMenu[1]);

						LCD_CursorOnOff(1);
						LCD_SetCursor( 15, 0 );

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
