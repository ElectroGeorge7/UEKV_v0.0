/*
 * lps_task.c
 *
 *  Created on: Mar 12, 2023
 *      Author: George
 */


#include "lps_task.h"

#include "rs485_hardware.h"

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#define LPS_MIN_ADDR	1
#define LPS_MAX_ADDR	32

#define LPS_RESP_MAX_WAIT_TIME	300

typedef struct {
	uint8_t updateReadyFlag;
	uint8_t conNum;
	uint8_t conAddrsList[32];
	LpsStatus_t *statusArray;
} LpsStatusList_t;

static LpsStatusList_t lpsStatusList = {0};

void lps_respond_handler(void){
	osSemaphoreRelease(lpsRespondSem);
}

void LpsTask(void *argument){

	// register the lps_respond_handler() as interrupt handler
	rs485_init();
	rs485_reg_respond_handler(lps_respond_handler);





	if ( osEventFlagsWait(testEvents, LPS_FIND_CONNECTED_START, osFlagsNoClear, osWaitForever) & LPS_FIND_CONNECTED_START ){

		// find connected lpses
		HAL_StatusTypeDef res = HAL_ERROR;
		char ADR_cmd[9] = {0};
		char lpsMdlStr[30] = {0};



		lpsStatusList.conNum = 0;
		memset(lpsStatusList.conAddrsList, 0, sizeof(lpsStatusList.conAddrsList));

		for (uint8_t addr = LPS_MIN_ADDR; addr <= LPS_MAX_ADDR; addr++){
			if ( addr < 10 )
				snprintf(ADR_cmd, sizeof(ADR_cmd), ":ADR0%1d;\r", addr);
			else
				snprintf(ADR_cmd, sizeof(ADR_cmd), ":ADR%2d;\r", addr);

			rs485_transmit(ADR_cmd, sizeof(ADR_cmd), addr+1);

			memset(lpsMdlStr, 0, sizeof(lpsMdlStr));
			res = lps_transmit_w_respond(MDL_cmd, sizeof(MDL_cmd), lpsMdlStr, strlen(lpsNames[0]), 2);

			if (res == HAL_OK)
				for (uint8_t i = 0; i < LPS_NAMES_NUM; i++){
					if ( strncmp(lpsMdlStr, lpsNames[i], strlen(lpsNames[i])) == 0 ){
						lpsList->conAddrsList[lpsList->conNum++] = addr;
						// toggle lps output to check the control access
						lps_ctrl_output(addr, LPS_OUTPUT_ON);
						HAL_Delay(3000);
						lps_ctrl_output(addr, LPS_OUTPUT_OFF);
					}
				}




		}


		osEventFlagsSet(testEvents, LPS_FIND_CONNECTED_FINISHED);
	}








	osRes = osSemaphoreAcquire(lpsRespondSem, 300);
	// init the lpsStatusList


	for(;;){

		if ( osEventFlagsWait(testEvents, LPS_LIST_UDATE_START, osFlagsNoClear, osWaitForever) & LPS_LIST_UDATE_START ){
			// get addr from list
			// transmit addr string
			// wait some definite time for respond
			// if time is out write zeros to correspond list item
			rs485_transmit(ADR_cmd, sizeof(ADR_cmd), addr+1);
		}

		osThreadYield();
		//osDelay(1);
	}
}


HAL_StatusTypeDef lps_transmit_w_respond(uint8_t *cmd, uint16_t txSize, uint8_t *respond, uint16_t rxSize, uint8_t repeat){
	HAL_StatusTypeDef res = HAL_ERROR;
	osStatus_t osRes = osError;
	char rxDmaBuf[60] = {0};

	while(repeat){

		res = rs485_tx(cmd, txSize, 1);
		res = rs485_rx_start(rxDmaBuf, rxSize, 1);

		osRes = osSemaphoreAcquire(lpsRespondSem, LPS_RESP_MAX_WAIT_TIME);
		rs485_rx_stop();

		if ( osRes == osOK ){
			memcpy(respond, rxDmaBuf, rxSize);
			return HAL_OK;
		} else {
			repeat--;
		}
	}

	return res;
}
