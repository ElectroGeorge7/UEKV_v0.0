/*
 * ub_check_task.c
 *
 *  Created on: Jan 7, 2023
 *      Author: George
 */

#include "ub_check_task.h"

#include "main.h"
#include "terminal.h"

#include "cmsis_os2.h"

#include "result_check.h"
#include "ub_check.h"
#include "leds_matrix.h"

#include "rtc_hardware.h"

extern uint16_t ledsBitMatrix[];

static uint32_t logNum = 0;

extern osSemaphoreId_t ubCheckSem;
extern osMessageQueueId_t logQueueHandler;

void UbCheckTask(void *argument){
	osStatus_t osRes;
	Log_t curLog = {0};

	for(;;){

		osRes = osSemaphoreAcquire(ubCheckSem, osWaitForever);

		uartprintf("TIM9 interrupt time: %d", HAL_GetTick());

		ub_check();
		for (uint8_t rowNum = 0; rowNum < UB_MATRIX_ROW_NUM; rowNum++){
			resultMatrix[rowNum] |= ubMatrix[rowNum];
		}
		ub_res_clear();

		leds_matrix_clear();
		HAL_Delay(50);
		memcpy(ledsBitMatrix, resultMatrix, sizeof(resultMatrix));
		//ledsBitMatrix[1] = 0xc0;
		//ledsBitMatrix[7] = 0x0c;
		leds_matrix_show_result();

		curLog.index = logNum++;

		DataTime_t dataTime;
		rtc_get(&dataTime);
		curLog.dataTime.sec = dataTime.sec;
		curLog.dataTime.min = dataTime.min;
		curLog.dataTime.hour = dataTime.hour;
		curLog.dataTime.day = dataTime.day;
		curLog.dataTime.mon = dataTime.mon;
		curLog.dataTime.year = dataTime.year;

		memcpy(curLog.result, resultMatrix, sizeof(resultMatrix));
		curLog.temp[0] = 125.125;
		curLog.temp[1] = 150.375;
		curLog.supplyCurrents[0].intVal = 1;
		curLog.supplyCurrents[0].fracVal = 15;
		curLog.supplyVoltages[0].intVal = 3;
		curLog.supplyVoltages[0].fracVal = 6;

		osRes = osMessageQueuePut(logQueueHandler, &curLog, 0, 0);

		result_check_clear();
		HAL_GPIO_TogglePin(GPIOC, LED_PROCESS_Pin);

		osThreadYield();
	}
}
