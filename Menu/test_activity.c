/*
 * reliability_test_activity.c
 *
 *  Created on: 25 янв. 2023 г.
 *      Author: Giorgi
 */

#include "test_activity.h"

#include "cmsis_os2.h"
#include "main.h"
#include "terminal.h"
#include <stdio.h>

#include "ts_spi.h"
#include "LCD1602.h"

#include "ub_check_task.h"
#include "ett_check_task.h"


#define TEST_ACT_TEST_IS_ACTIVE 	0x01
#define TEST_ACT_CONFIG_IS_SET 		0x02
#define TEST_ACT_CONFIG_TO_WRITE 	0x04
static uint8_t testActStatusFlags = 0;

#define TEST_ACT_MENU_ROW_NUM 14
static uint8_t curMenuRow = 0;
static uint8_t curCursorPos = 0;
static char testActMenu[TEST_ACT_MENU_ROW_NUM][32] = {0}; // cyrillic letters take 2 bytes
TestConfig_t curConfig = {0};

#define TEST_CONFIG_PART_NUM_IS_SET				0x01
#define TEST_CONFIG_MLDR_NUM_IS_SET				0x02
#define TEST_CONFIG_TEST_TYPE_IS_SET			0x04
#define TEST_CONFIG_MATRIX_IS_SET				0x10
#define TEST_CONFIG_RES_CHECK_METHOD_IS_SET		0x20
#define TEST_CONFIG_RES_CHECK_PERIOD_IS_SET		0x40
#define TEST_CONFIG_PCB_NUM_IS_SET				0x100

extern osThreadId_t ubCheckTaskHandle;
extern const osThreadAttr_t ubCheckTask_attributes;

extern osThreadId_t ettCheckTaskHandle;
extern const osThreadAttr_t ettCheckTask_attributes;

extern osEventFlagsId_t testEvents;
extern osMessageQueueId_t eventQueueHandler;

HAL_StatusTypeDef test_view_update(Command_t testAction, uint8_t *data){
	char pBuf[16] = {0};
	DataTime_t dataTime = {0};
	Event_t msg = {0};
	osStatus_t res;
	Log_t *pLog = {0};

	uint32_t osEventFlag = 0;

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
					LCD_SetCursor( 15, curCursorPos = 1 );
				}else{
					LCD_SetCursor( 15, curCursorPos = 0 );
				}
			}
            break;
        case DOWN_CMD:
        	if ( (curMenuRow+1) < TEST_ACT_MENU_ROW_NUM){
        		curMenuRow++;
				if ( curMenuRow%2 ){
					LCD_SetCursor( 15, curCursorPos = 1 );
				}else{
					LCD_Clear();
					LCD_SetCursor( 0, 0 );
					LCD_PrintString(testActMenu[curMenuRow]);
					LCD_SetCursor( 0, 1 );
					LCD_PrintString(testActMenu[curMenuRow+1]);
					LCD_SetCursor( 15, curCursorPos = 0 );
				}
        	}
            break;
		case SELECT_CMD:

			if ( !(testActStatusFlags & TEST_ACT_TEST_IS_ACTIVE) ){
				if ( !(testActStatusFlags & TEST_ACT_CONFIG_IS_SET) ){

					LCD_Clear();
					LCD_CursorOnOff(0);
					LCD_SetCursor( 0, 0 );
					LCD_PrintString("Поиск");
					LCD_SetCursor( 0, 1 );
					LCD_PrintString("test_cfg.txt");
					HAL_Delay(1000);

//////////////////// убрать после отладки
					//osEventFlagsSet(testEvents, LPS_FIND_CONNECTED_START);
					//osEventFlag = osEventFlagsWait(testEvents, LPS_FIND_CONNECTED_FINISHED, osFlagsWaitAny, osWaitForever);
///////////////////////////////////////////////////////////////////////////////////////////////////////////////

					osEventFlagsSet(testEvents, TEST_CONFIG_SEARCH);
					if ( (osEventFlag = osEventFlagsWait(testEvents, TEST_CONFIG_IS_FIND | TEST_CONFIG_IS_NOT, osFlagsWaitAny, osWaitForever)) & TEST_CONFIG_IS_FIND ){

						// wait for config from file
						if( osMessageQueueGet(eventQueueHandler, &msg, NULL, osWaitForever) == osOK ){
							if ( msg.event == TEST_CONFIG_SEND ){
								memcpy((uint8_t *)&curConfig, msg.eventStr, sizeof(TestConfig_t));
							}
						}

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
					} else {
						LCD_SetCursor( 0, 1 );
						LCD_PrintString("не найдено      ");
						HAL_Delay(1000);

						LCD_Clear();
						LCD_SetCursor( 0, 0 );
						LCD_PrintString("Настр-ка по USB");
						usbprintf("Test config set. Enter Part Number (up to 16 symbols).");
						// after terminal config it`s needed to write config in log file
						testActStatusFlags |= TEST_ACT_CONFIG_TO_WRITE;
					}

				} else {
					osEventFlagsSet(testEvents, TEST_START);
					osThreadYield();
					osDelay(1);
					osEventFlagsSet(testEvents, LPS_LIST_UPDATE_START);
					osThreadYield();
					osDelay(1);

					switch(curConfig.testType){
					case RELIABILITY_TEST:
						 ubCheckTaskHandle = osThreadNew(UbCheckTask, NULL, &ubCheckTask_attributes);
						break;
					case ETT_TEST:
						ettCheckTaskHandle = osThreadNew(EttCheckTask, NULL, &ettCheckTask_attributes);
						break;
					default:
						usbprintf("Incorrect test was set.");
						activity_change(BACK_CMD);
					}

					result_check_init(curConfig);
					testActStatusFlags |= TEST_ACT_TEST_IS_ACTIVE;
				}
			}

			break;
		case BACK_CMD:
			result_check_deinit(curConfig);
			testActStatusFlags = 0;
			testConfigsSetFlag = 0;
			osEventFlagsSet(testEvents, TEST_FINISH);

			switch(curConfig.testType){
			case RELIABILITY_TEST:
				res = osThreadTerminate(ubCheckTaskHandle);
				if (res != osOK) {
					usbprintf("Incorrect ubCheckTask termination.");
				}
				break;
			case ETT_TEST:
				res = osThreadTerminate(ettCheckTaskHandle);
				if (res != osOK) {
					usbprintf("Incorrect ettCheckTask termination.");
				}
				break;
			default:
				usbprintf("Incorrect test was set.");
				activity_change(BACK_CMD);
			}

			//osThreadYield();
			activity_change(MENU_ACTIVITY);
			break;
		case UPDATE_CMD:
			if ( testActStatusFlags & TEST_ACT_TEST_IS_ACTIVE ){
				if ( (osEventFlag = osEventFlagsWait(testEvents, TEST_LOG_DISPLAY, osFlagsWaitAny, 0)) & TEST_LOG_DISPLAY ){

						pLog = (Log_t *)(data);

						snprintf(testActMenu[1], 32, "Рез.: %d", pLog->index);
						// current time
						snprintf(testActMenu[9], 32, "%d:%d %d.%d.%d", pLog->dataTime.hour, pLog->dataTime.min, pLog->dataTime.day, pLog->dataTime.mon, pLog->dataTime.year);

						float temp1 = pLog->temp[0];
						float temp2 = pLog->temp[1];
						snprintf(testActMenu[18], 32, "темп.1: %3.0f C", temp1);
						snprintf(testActMenu[19], 32, "темп.2: %3.0f C", temp2);

						LCD_Clear();
						LCD_SetCursor( 0, 0 );
						LCD_PrintString(testActMenu[curMenuRow]);
						LCD_SetCursor( 0, 1 );
						LCD_PrintString(testActMenu[curMenuRow+1]);
						LCD_SetCursor( 15, curCursorPos = 0 );

						osEventFlagsSet(testEvents, TEST_LOG_PROCCESS_FINISHED);
					}
			}
			break;
		case TERMINAL_CMD:
			if ( !(testActStatusFlags & TEST_ACT_TEST_IS_ACTIVE) ){
				if ( !(testActStatusFlags & TEST_ACT_CONFIG_IS_SET) ){
					if ( test_terminal_config(data, &curConfig, &testConfigsSetFlag) == HAL_OK ){

						if (testActStatusFlags & TEST_ACT_CONFIG_TO_WRITE){
							msg.event = TEST_CONFIG_SEND;
							memcpy(msg.eventStr, (uint8_t *)&curConfig, sizeof(TestConfig_t));
							osMessageQueuePut(eventQueueHandler, &msg, 0, 0);
							osEventFlagsSet(testEvents, TEST_CONFIG_SEND);
						}


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
				}
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
	int resCheckPeriod = 0;
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
		sscanf(data, "%1d", &testType);
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
		usbprintf("0 - average result,");
		usbprintf("1 - synchro result for UB,");
		*flag |= TEST_CONFIG_MATRIX_IS_SET;
	} else if ( !(*flag & TEST_CONFIG_RES_CHECK_METHOD_IS_SET) ){
		sscanf(data, "%1d", &resCheckMethod);
		curConfig->resCheckMethod = resCheckMethod;
		usbprintf("Result check method: %d", curConfig->resCheckMethod);
		usbprintf("Enter result check period: ( 1-15s, check period just for average check )");
		*flag |= TEST_CONFIG_RES_CHECK_METHOD_IS_SET;
	} else if ( !(*flag & TEST_CONFIG_RES_CHECK_PERIOD_IS_SET) ){
		sscanf(data, "%2d", &resCheckPeriod);
		curConfig->resCheckPeriod = resCheckPeriod;
		usbprintf("Result check period: %d", curConfig->resCheckPeriod);
		usbprintf("Enter number of PCBs, just for ETT (up to 4 PCBs).");
		*flag |= TEST_CONFIG_RES_CHECK_PERIOD_IS_SET;
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
		SYNCHRO_RESULT,
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

	snprintf(testActMenu[6], 32, "Метод считыв.:");
	snprintf(testActMenu[7], 32, "%s", curConfig->resCheckMethod ? "синхро" : "среднее" );

	snprintf(testActMenu[8], 32, "тек. время:");
	rtc_get(&dataTime);
	snprintf(testActMenu[9], 32, "%d:%d %d.%d.%d", dataTime.hour, dataTime.min, dataTime.day, dataTime.mon, dataTime.year);

	float temp1 = ts_check(1);
	float temp2 = ts_check(2);
	snprintf(testActMenu[10], 32, "темп.1:%3.0fC", temp1);
	snprintf(testActMenu[11], 32, "темп.2:%3.0fC", temp2);

	snprintf(testActMenu[12], 32, "кол-во плат:%d", curConfig->pcbNum);
	memset(testActMenu[13], 0, 32);
}

void test_reg_activity_cb(ActivityViewUpdateCb_t *pTestUpdateCb){
    *pTestUpdateCb = test_view_update;
}
