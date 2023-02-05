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


#define TEST_ACT_TEST_IS_ACTIVE 0x01
#define TEST_ACT_CONFIG_IS_SET 0x02
static uint8_t testActStatusFlags = 0;

#define TEST_ACT_MENU_ROW_NUM 22
static uint8_t curMenuRow = 0;
static char testActMenu[TEST_ACT_MENU_ROW_NUM][32] = {0}; // cyrillic letters take 2 bytes
TestConfig_t curConfig = {0};

#define TEST_CONFIG_PART_NUM_IS_SET				0x01
#define TEST_CONFIG_MLDR_NUM_IS_SET				0x02
#define TEST_CONFIG_TEST_TYPE_IS_SET			0x04
#define TEST_CONFIG_MATRIX_IS_SET				0x10
#define TEST_CONFIG_RES_CHECK_METHOD_IS_SET		0x20
#define TEST_CONFIG_TEST_DUR_IS_SET				0x40
#define TEST_CONFIG_POW_SUP_NUM_IS_SET			0x80
#define TEST_CONFIG_PCB_NUM_IS_SET				0x100


HAL_StatusTypeDef test_view_update(Command_t testAction, uint8_t *data){
	char pBuf[16] = {0};
	DataTime_t dataTime = {0};

	static uint16_t testConfigsSetFlag = 0;

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


					if (0/* check config file in file system */){
						// get configuration from the file
						testActStatusFlags |= TEST_ACT_CONFIG_IS_SET;
					} else {
						LCD_SetCursor( 0, 1 );
						LCD_PrintString("не найдено");
						HAL_Delay(1000);

						LCD_Clear();
						LCD_SetCursor( 0, 0 );
						LCD_PrintString("Настр-ка по USB");
						usbprintf("Test config set. Enter Part Number (up to 16 symbols).");
					}

				} else {
					// wait until usb config
					// after config info was received display it and start test();
					result_check_init();
				}
			}

			break;
		case BACK_CMD:
			result_check_deinit();
			testActStatusFlags = 0;
			testConfigsSetFlag = 0;
			activity_change(MENU_ACTIVITY);
			break;
		case TERMINAL_CMD:
			if ( test_terminal_config(data, &curConfig, &testConfigsSetFlag) == HAL_OK ){

				test_menu_update(&curConfig);

				LCD_Clear();
				curMenuRow = 0;

				LCD_SetCursor( 0, 0 );
				LCD_PrintString(testActMenu[0]);
				LCD_SetCursor( 0, 1 );
				LCD_PrintString(testActMenu[1]);

				LCD_CursorOnOff(1);
				LCD_SetCursor( 15, 0 );

				testActStatusFlags |= TEST_ACT_CONFIG_IS_SET;
			}
			break;
	  default:
			break;
	}

	return HAL_OK;
}

HAL_StatusTypeDef test_terminal_config(uint8_t *data, TestConfig_t *curConfig, uint16_t *flag){

	int testType = 0;
	int cellNum = 0;
	int rowNum = 0;
	int colNum = 0;
	int resCheckMethod = 0;
	int testDurationInHours = 0;
	int powerSupplyNum = 0;
	int pcbNum = 0;


	if ( !(*flag & TEST_CONFIG_PART_NUM_IS_SET) ){
		sscanf(data, "%16s", curConfig->partNumber);
		usbprintf("Part Number: %s", curConfig->partNumber);
		usbprintf("Enter MLDR number in format: MLDRxxxx.");
		*flag |= TEST_CONFIG_PART_NUM_IS_SET;
	} else if ( !(*flag & TEST_CONFIG_MLDR_NUM_IS_SET) ){
		sscanf(data, "%9s", curConfig->mldrNum);
		usbprintf("MLDR number: %s", curConfig->mldrNum);
		usbprintf("Enter type of test: 0 - Reliability, 1 - ETT.");
		*flag |= TEST_CONFIG_MLDR_NUM_IS_SET;
	} else if ( !(*flag & TEST_CONFIG_TEST_TYPE_IS_SET) ){
		sscanf(data, "%d", &testType);
		curConfig->testType = testType;
		usbprintf("Type of test: %s", curConfig->testType ? "ETT" : "Reliability");
		usbprintf("Enter numbers of used cells, rows and columns in format: cell:xxx row:xx col:xx");
		*flag |= TEST_CONFIG_TEST_TYPE_IS_SET;
	} else if ( !(*flag & TEST_CONFIG_MATRIX_IS_SET) ){
		sscanf(data, "cell:%3d row:%2d col:%2d", &cellNum, &rowNum, &colNum);
		curConfig->cellNum = cellNum;
		curConfig->rowNum = rowNum;
		curConfig->colNum = colNum;
		usbprintf("cell:%d row:%d col:%d", curConfig->cellNum, curConfig->rowNum, curConfig->colNum);
		usbprintf("Enter result check method:");
		usbprintf("0 - every result,");
		usbprintf("1 - average result during 1s period,");
		usbprintf("2 - average result during 2s period,");
		usbprintf("3 - average result during 3s period,");
		usbprintf("4 - average result during 4s period,");
		usbprintf("5 - average result during 5s period,");
		usbprintf("6 - just faults");
		*flag |= TEST_CONFIG_MATRIX_IS_SET;
	} else if ( !(*flag & TEST_CONFIG_RES_CHECK_METHOD_IS_SET) ){
		sscanf(data, "%d", &resCheckMethod);
		curConfig->resCheckMethod = resCheckMethod;
		usbprintf("Result check method: %d", curConfig->resCheckMethod);
		usbprintf("Enter test duration in hours (up to 99999 hours).");
		*flag |= TEST_CONFIG_RES_CHECK_METHOD_IS_SET;
	} else if ( !(*flag & TEST_CONFIG_TEST_DUR_IS_SET) ){
		sscanf(data, "%5d", &testDurationInHours);
		curConfig->testDurationInHours = testDurationInHours;
		usbprintf("Test duration in hours: %d h", curConfig->testDurationInHours);
		usbprintf("Enter number of power supplies (up to 32).");
		*flag |= TEST_CONFIG_TEST_DUR_IS_SET;
	} else if ( !(*flag & TEST_CONFIG_POW_SUP_NUM_IS_SET) ){
		sscanf(data, "%2d", &powerSupplyNum);
		curConfig->powerSupplyNum = powerSupplyNum;
		usbprintf("Number of power supplies: %d", curConfig->powerSupplyNum);
		usbprintf("Enter number of PCBs, just for ETT (up to 4 PCBs).");
		*flag |= TEST_CONFIG_POW_SUP_NUM_IS_SET;
	} else if ( !(*flag & TEST_CONFIG_PCB_NUM_IS_SET) ){
		sscanf(data, "%d", &pcbNum);
		curConfig->pcbNum = pcbNum;
		usbprintf("Number of PCBs: %d", curConfig->pcbNum);
		usbprintf("End of test configuration by terminal.");
		usbprintf("Press SEL to start test, BACK - to stop test, UP/DOWN - menu moving");
		*flag |= TEST_CONFIG_PCB_NUM_IS_SET;
		return HAL_OK;
	}

	return HAL_ERROR;
}

void test_menu_update(TestConfig_t *curConfig){
/*
	TestConfig_t curConfig = {
		"qwerty1234",
		"MLDR123",
		RELIABILITY_TEST,
		12,
		4,
		3,
		EVERY_RESULT,
		{0, 30, 14, 12, 2, 2023 },
		10000,
		1,
		1
	};
*/
	DataTime_t dataTime = {0};

	int hour = 0;
	int min = 0;
	int day = 0;
	int mon = 0;
	int year = 0;

	snprintf(testActMenu[0], 32, "%s", curConfig->partNumber);
	snprintf(testActMenu[1], 32, "Рез.:0");

	snprintf(testActMenu[2], 32, "%s  %dшт.", curConfig->mldrNum, curConfig->cellNum);
	snprintf(testActMenu[3], 32, "row:%d    col:%d", curConfig->rowNum, curConfig->colNum);

	snprintf(testActMenu[4], 32, "Испытание:");
	snprintf(testActMenu[5], 32, "%s", curConfig->testType ? "ЭТТ" : "Безотказность");

	snprintf(testActMenu[6], 32, "Считыв. рез-та:");
	snprintf(testActMenu[7], 32, "%d", curConfig->resCheckMethod);

	snprintf(testActMenu[8], 32, "тек. время:");
	rtc_get(&dataTime);
	snprintf(testActMenu[9], 32, "%d:%d %d.%d.%d", dataTime.hour, dataTime.min, dataTime.day, dataTime.mon, dataTime.year);

	snprintf(testActMenu[10], 32, "начало испыт-ия:");
	memcpy(&(curConfig->testStartDataTime), &dataTime, sizeof(DataTime_t));
	snprintf(testActMenu[11], 32, "%d:%d %d.%d.%d", dataTime.hour, dataTime.min, dataTime.day, dataTime.mon, dataTime.year);

	snprintf(testActMenu[12], 32, "конец испыт-ия:");
	snprintf(testActMenu[13], 32, "%d:%d %d.%d.%d", 5, 0, 7, 5, 2023);

	snprintf(testActMenu[14], 32, "осталось:");
	snprintf(testActMenu[15], 32, "%d:%d %d.%d.%d", 5, 3, 2, 3, 0);

	snprintf(testActMenu[16], 32, "кол-во Vпит:%d", curConfig->powerSupplyNum);
	memset(testActMenu[17], 0, 32);

	float temp1 = Max6675_Read_Temp();
	float temp2 = Max6675_Read_Temp();
	snprintf(testActMenu[18], 32, "темп.1:%3.0fC", temp1);
	snprintf(testActMenu[19], 32, "темп.2:%3.0fC", temp2);

	snprintf(testActMenu[20], 32, "кол-во плат:%d", curConfig->pcbNum);
	memset(testActMenu[21], 0, 32);
}

void test_reg_activity_cb(ActivityViewUpdateCb_t *pTestUpdateCb){
    *pTestUpdateCb = test_view_update;
}
