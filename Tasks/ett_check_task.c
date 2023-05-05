/*
 * ett_check_task.c
 *
 *  Created on: 24 апр. 2023 г.
 *      Author: George
 */


#include "ett_check_task.h"

#include "main.h"
#include "terminal.h"
#include "reliability.h"

#include "cmsis_os2.h"

#include <stdlib.h>

#include "result_check.h"
#include "ett_check.h"
#include "leds_matrix.h"
#include "ts_spi.h"

#include "rtc_hardware.h"
#include "lps_task.h"

#define UEKV_DEBUG_ETT_TEST	1

static uint32_t logNum = 0;

extern osSemaphoreId_t resCheckSem;
extern osMessageQueueId_t logQueueHandler;
extern osEventFlagsId_t testEvents;


void EttCheckTask(void *argument){
	osStatus_t osRes;
	Log_t curLog = {0};
	LpsStatus_t *ubLpsList = NULL;

	uint8_t lpsNum = 0;
	uint32_t osEventFlag = 0;

	if ( bkp_read_data(UEKV_LAST_STATE_REG) == UEKV_TEST_STATE ){
		logNum = bkp_read_data(UEKV_LAST_TEST_RES_NUM_REG);
	}

#if ( UEKV_DEBUG_ETT_TEST )
		uint16_t dbgStartLogNum = logNum + 1;	// execute module test after first result check
		/*
		 * 	dbgEttTestNum = 1 - check ett indication from 1x1 to 7x5 matrix of 0x55/0xaa for 1-4 pcbs,
		 * 	119*4 result indications once in 1s
		 * 	dbgEttTestNum = 2 - 5 times of zero result indication for 7x5 matrix with 4 pcbs once in 1s
		 * 	dbgEttTestNum = 3 - 5 times of indication of full fill 7x5 matrix with 4 pcbs once in 2s,
		 * 	each result update on ett board last for 1s
		 * 	dbgEttTestNum = 4 - 5 times of indication of full fill 7x5 matrix with 4 pcbs once in 3s,
		 * 	each result update on ett board last for 2s
		 * 	dbgEttTestNum = 5 - 5 times of indication of full fill 7x5 matrix with 4 pcbs once in 4s,
		 * 	each result update on ett board last for 3s
		 * 	dbgEttTestNum = 6 - 5 times of indication of full fill 7x5 matrix with 4 pcbs once in 5s,
		 * 	each result update on ett board last for 4s
		 */
		uint8_t dbgEttTestNum = 1;
		TestConfig_t dbgConfig = {0};
		dbgConfig.cellNum = 1;
		dbgConfig.rowNum = 1;
		dbgConfig.colNum = 1;
		dbgConfig.pcbNum = 1;
		dbgConfig.testType = ETT_TEST;
		dbgConfig.resCheckMethod = AVERAGE_RESULT_PER_1S;
#endif

	for(;;){

		osRes = osSemaphoreAcquire(resCheckSem, osWaitForever);

		uartprintf("TIM9 interrupt time: %d", HAL_GetTick());

#if ( UEKV_DEBUG_ETT_TEST )
		if ( logNum == dbgStartLogNum ){
			result_check_deinit();
			result_check_init(dbgConfig);
		} else if ( logNum > dbgStartLogNum ) {

			switch(dbgEttTestNum){
			case 1:
				if ( dbgConfig.cellNum < 35 ){
					if ( dbgConfig.cellNum < 5*dbgConfig.colNum ){
						dbgConfig.cellNum++;
						if ( dbgConfig.cellNum > dbgConfig.rowNum*dbgConfig.colNum )
							dbgConfig.rowNum++;
					} else {
						dbgConfig.colNum++;
						dbgConfig.rowNum = 1;
						dbgConfig.cellNum = dbgConfig.colNum;
					}
				} else {
					if ( dbgConfig.pcbNum < 4 ){
						dbgConfig.pcbNum++;
						dbgConfig.cellNum = 1;
						dbgConfig.rowNum = 1;
						dbgConfig.colNum = 1;
					} else {
						dbgEttTestNum++;
						dbgConfig.pcbNum = 4;
						dbgConfig.cellNum = 35;
						dbgConfig.rowNum = 5;
						dbgConfig.colNum = 7;
						dbgStartLogNum = logNum;
					}
				}
				break;
			case 2:
				if ( (logNum-dbgStartLogNum) > (5-1) ){
					dbgEttTestNum++;
					dbgConfig.pcbNum = 4;
					dbgConfig.cellNum = 35;
					dbgConfig.rowNum = 5;
					dbgConfig.colNum = 7;
					dbgConfig.resCheckMethod = AVERAGE_RESULT_PER_2S;
					dbgStartLogNum = logNum;
				}
				break;
			case 3:
				if ( (logNum-dbgStartLogNum) > (5-1) ){
					dbgEttTestNum++;
					dbgConfig.pcbNum = 4;
					dbgConfig.cellNum = 35;
					dbgConfig.rowNum = 5;
					dbgConfig.colNum = 7;
					dbgConfig.resCheckMethod = AVERAGE_RESULT_PER_3S;
					dbgStartLogNum = logNum;
				}
				break;
			case 4:
				if ( (logNum-dbgStartLogNum) > (5-1) ){
					dbgEttTestNum++;
					dbgConfig.pcbNum = 4;
					dbgConfig.cellNum = 35;
					dbgConfig.rowNum = 5;
					dbgConfig.colNum = 7;
					dbgConfig.resCheckMethod = AVERAGE_RESULT_PER_4S;
					dbgStartLogNum = logNum;
				}
				break;
			case 5:
				if ( (logNum-dbgStartLogNum) > (5-1) ){
					dbgEttTestNum++;
					dbgConfig.pcbNum = 4;
					dbgConfig.cellNum = 35;
					dbgConfig.rowNum = 5;
					dbgConfig.colNum = 7;
					dbgConfig.resCheckMethod = AVERAGE_RESULT_PER_5S;
					dbgStartLogNum = logNum;
				}
				break;
			case 6:
				if ( (logNum-dbgStartLogNum) > (5-1) ){
					dbgEttTestNum = 1;
					dbgConfig.pcbNum = 1;
					dbgConfig.cellNum = 1;
					dbgConfig.rowNum = 1;
					dbgConfig.colNum = 1;
					dbgConfig.resCheckMethod = AVERAGE_RESULT_PER_1S;
					dbgStartLogNum = logNum;
				}
				break;
			}

			result_check_deinit();
			result_check_init(dbgConfig);
		}
#endif

		ett_check(resultMatrix);
		result_show(resultMatrix);

		curLog.index = logNum++;
		bkp_write_data(UEKV_LAST_TEST_RES_NUM_REG, logNum);

		DataTime_t dataTime;
		rtc_get(&dataTime);
		curLog.dataTime.sec = dataTime.sec;
		curLog.dataTime.min = dataTime.min;
		curLog.dataTime.hour = dataTime.hour;
		curLog.dataTime.day = dataTime.day;
		curLog.dataTime.mon = dataTime.mon;
		curLog.dataTime.year = dataTime.year;

		memcpy(curLog.result, resultMatrix, sizeof(resultMatrix));
		curLog.temp[0] = ts_check(1);
		curLog.temp[1] = ts_check(2);


		if (lpsNum == 0){
			lpsNum = lps_get_connected_num();
			if ( (lpsNum >= 1) && (lpsNum<=32) ){
				ubLpsList = (LpsStatus_t *) calloc(lpsNum, sizeof(LpsStatus_t));
			}
		}

		if (ubLpsList != NULL){
			if ( lps_get_update_reade_flag() ){
				// get the lps list when it is ready and copy it to the local buf
				memcpy(ubLpsList, lps_list_get(), lpsNum * sizeof(LpsStatus_t));
				osEventFlagsSet(testEvents, LPS_LIST_UDATE_START);
				curLog.lpsStatusArray = ubLpsList;
			} else {
				curLog.lpsStatusArray = ubLpsList;
			}
		}

		osRes = osMessageQueuePut(logQueueHandler, &curLog, 0, 0);
		osEventFlagsSet(testEvents, TEST_LOG_SAVE);

		result_check_clear();
		HAL_GPIO_TogglePin(GPIOC, LED_PROCESS_Pin);

		osThreadYield();
	}
}
