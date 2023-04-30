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

extern uint16_t ledsBitMatrix[];

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

	for(;;){

		osRes = osSemaphoreAcquire(resCheckSem, osWaitForever);

		uartprintf("TIM9 interrupt time: %d", HAL_GetTick());

		ett_check();

		for (uint8_t rowNum = 0; rowNum < ETT_MATRIX_ROW_NUM; rowNum++){
			resultMatrix[rowNum] |= ettMatrix[rowNum];
		}
		ett_res_clear();


		leds_matrix_clear();
		HAL_Delay(50);
		memcpy(ledsBitMatrix, resultMatrix, sizeof(resultMatrix));
		leds_matrix_show_result();


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
