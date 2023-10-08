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
#include "status_leds.h"
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

	ResCheckMethod_t ubCheckMeth = ub_check_method_get();

	if ( bkp_read_data(UEKV_LAST_STATE_REG) == UEKV_TEST_STATE ){
		logNum = bkp_read_data(UEKV_LAST_TEST_RES_NUM_REG);
	}

	for(;;){

		osRes = osSemaphoreAcquire(resCheckSem, osWaitForever);

		uartprintf("TIM9 interrupt time: %d", HAL_GetTick());

		if ( ubCheckMeth == AVERAGE_RESULT ){
			ub_check_aver_finish(resultMatrix);
		} else if ( ubCheckMeth == SYNCHRO_RESULT ){
			// wait for result showing pause
			uint16_t timeout = 150;
			uint32_t timeoutStart = osKernelGetTickCount();
			while ( osKernelGetTickCount() - timeoutStart < timeout){
				if (HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_15) != GPIO_PIN_SET)
					timeoutStart = osKernelGetTickCount();
			}
			ub_check_aver_finish(resultMatrix);
		}
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
			if ( lps_get_update_ready_flag() ){
				// get the lps list when it is ready and copy it to the local buf
				memcpy(ubLpsList, lps_list_get(), lpsNum * sizeof(LpsStatus_t));
				osEventFlagsSet(testEvents, LPS_LIST_UPDATE_START);
				curLog.lpsStatusArray = ubLpsList;
			} else {
				curLog.lpsStatusArray = ubLpsList;
			}
		}

		osRes = osMessageQueuePut(logQueueHandler, &curLog, 0, 0);
		osEventFlagsSet(testEvents, TEST_LOG_SAVE);

		result_check_clear();
		status_leds_toggle(LED_PROCESS_Pin);

		if ( (osEventFlag = osEventFlagsWait(testEvents, TEST_LOG_PROCCESS_FINISHED, osFlagsWaitAny, osWaitForever)) & TEST_LOG_PROCCESS_FINISHED ){
			if ( ubCheckMeth == AVERAGE_RESULT ){
				ub_check_aver_start();
			} else if ( ubCheckMeth == SYNCHRO_RESULT ){
				__HAL_GPIO_EXTI_CLEAR_IT(GPIO_PIN_15);
				NVIC_ClearPendingIRQ (EXTI15_10_IRQn);
				HAL_NVIC_SetPriority(EXTI15_10_IRQn, 5, 0);
				HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);
			}
		}
		osThreadYield();
	}
}
