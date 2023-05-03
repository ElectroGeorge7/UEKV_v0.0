/*
 * ub_check_task.c
 *
 *  Created on: Jan 7, 2023
 *      Author: George
 */

#include "ub_check_task.h"

#include "main.h"
#include "terminal.h"
#include "reliability.h"

#include "cmsis_os2.h"

#include <stdlib.h>

#include "result_check.h"
#include "ub_check.h"
#include "leds_matrix.h"
#include "ts_spi.h"

#include "rtc_hardware.h"
#include "lps_task.h"

static uint32_t logNum = 0;

extern osSemaphoreId_t resCheckSem;
extern osMessageQueueId_t logQueueHandler;
extern osEventFlagsId_t testEvents;


void UbCheckTask(void *argument){
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

		ub_check_dma_stop();

		//ub_check();
		for (uint8_t rowNum = 0; rowNum < UB_MATRIX_ROW_NUM; rowNum++){
			resultMatrix[rowNum] |= ubMatrix[rowNum];
		}

/*
 	 	 	// phase synchro
			rch_timer_stop();
			if ( ub_check_sig_level_wait(0, 1, 0xffff) == HAL_OK){
				rch_timer_start();
				uartprintf("phase alignment on the first signal of row1: ok");
				return HAL_OK;
			}
*/
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
		curLog.temp[1] = ts_check(2);;



		//curLog.supplyCurrents[0].intVal = 1;
		//curLog.supplyCurrents[0].fracVal = 15;
		//curLog.supplyVoltages[0].intVal = 3;
		//curLog.supplyVoltages[0].fracVal = 6;

		// get the lps`s data from
		//lps_read_status(1, curLog.lpsState, sizeof(curLog.lpsState));

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

		if ( (osEventFlag = osEventFlagsWait(testEvents, TEST_LOG_PROCCESS_FINISHED, osFlagsWaitAny, osWaitForever)) & TEST_LOG_PROCCESS_FINISHED ){
			ub_check_dma_start();
		}


		osThreadYield();
	}
}
