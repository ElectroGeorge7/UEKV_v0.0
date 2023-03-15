/*
 * lps_task.c
 *
 *  Created on: Mar 12, 2023
 *      Author: George
 */


#include "lps_task.h"

#include "cmsis_os2.h"

#include "main.h"
#include "rs485_hardware.h"

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

extern osSemaphoreId_t lpsRespondSem;
extern osEventFlagsId_t testEvents;

#define LPS_MIN_ADDR	1
#define LPS_MAX_ADDR	32

#define LPS_RESP_MAX_WAIT_TIME	300

#define LPS_STT_RESPOND_SIZE 	54
#define LPS_STT_VOL_START_BIT	10
#define LPS_STT_CUR_START_BIT	20
#define LPS_STT_OS_REG_OUT_BIT	33

#define LPS_NAMES_NUM	1
static const char *lpsNames[LPS_NAMES_NUM] = {
		"Nemic-Lambda ZUP(60V-7A)\r\n"
		/// @todo add the second type of lps
};

const uint8_t MDL_cmd[] = ":MDL?;\r";
const uint8_t REV_cmd[] = ":REV?;\r";
const uint8_t RMT0_cmd[] = ":RMT0;\r";
const uint8_t RMT1_cmd[] = ":RMT1;\r";
const uint8_t RMT2_cmd[] = ":RMT2;\r";
const uint8_t OUT1_cmd[] = ":OUT1;\r";
const uint8_t OUT0_cmd[] = ":OUT0;\r";
const uint8_t STT_cmd[] = ":STT?;\r";

typedef enum LpsOutputState{
	LPS_OUTPUT_OFF,
	LPS_OUTPUT_ON,
} LpsOutputState_t;

typedef struct {
	uint8_t updateReadyFlag;
	uint8_t conNum;
	uint8_t conAddrsList[32];
	LpsStatus_t *statusArray;
} LpsStatusList_t;

static LpsStatusList_t lpsStatusList = {0};



HAL_StatusTypeDef lps_tx_w_respond(uint8_t *cmd, uint16_t txSize, uint8_t *respond, uint16_t rxSize, uint8_t repeat);
HAL_StatusTypeDef lps_read_status(uint8_t addr, uint8_t *rxData, uint16_t size);
HAL_StatusTypeDef lps_ctrl_output(uint8_t addr, LpsOutputState_t state);
HAL_StatusTypeDef lps_find_connected(LpsStatusList_t *lpsList);
HAL_StatusTypeDef lps_list_init(void);



void lps_respond_handler(void){
	osSemaphoreRelease(lpsRespondSem);
}

void LpsTask(void *argument){
	HAL_StatusTypeDef res = HAL_ERROR;
	uint8_t lpsStatusBuf[LPS_STT_RESPOND_SIZE] = {0};
	LpsStatus_t *pStatusArray = NULL;
	

	// register the lps_respond_handler() as interrupt handler
	rs485_init();
	rs485_reg_respond_handler(lps_respond_handler);

	


	if ( osEventFlagsWait(testEvents, LPS_FIND_CONNECTED_START, osFlagsNoClear, osWaitForever) & LPS_FIND_CONNECTED_START ){

		lps_list_init();

		osEventFlagsSet(testEvents, LPS_FIND_CONNECTED_FINISHED);
	}



	for(;;){

		if ( osEventFlagsWait(testEvents, LPS_LIST_UDATE_START, osFlagsNoClear, osWaitForever) & LPS_LIST_UDATE_START ){

			memset(lpsStatusList.statusArray, 0, sizeof(lpsStatusList.statusArray));

			for (uint8_t i = 0; i < lpsStatusList.conNum; i++ ){
				res = lps_read_status(lpsStatusList.conAddrsList[i], lpsStatusBuf, LPS_STT_RESPOND_SIZE);
				if ( res == HAL_OK ){
					pStatusArray = &(lpsStatusList.statusArray[i]);
					pStatusArray->addr = lpsStatusList.conAddrsList[i];
					memcpy(pStatusArray->volStr, lpsStatusBuf[LPS_STT_VOL_START_BIT], sizeof(pStatusArray->volStr)-1);
					memcpy(pStatusArray->curStr, lpsStatusBuf[LPS_STT_CUR_START_BIT], sizeof(pStatusArray->curStr)-1);
				}
			}

			osEventFlagsClear(testEvents, LPS_LIST_UDATE_START);
			osEventFlagsSet(testEvents, LPS_LIST_UDATE_FINISHED);
		}

		osThreadYield();
		//osDelay(1);
	}
}

uint8_t lps_get_connected_num(void){
	return lpsStatusList.conNum;
}

LpsStatus_t *lps_list_get(void){
	return lpsStatusList.statusArray;
}


HAL_StatusTypeDef lps_tx_w_respond(uint8_t *cmd, uint16_t txSize, uint8_t *respond, uint16_t rxSize, uint8_t repeat){
	HAL_StatusTypeDef res = HAL_ERROR;
	osStatus_t osRes = osError;
	uint8_t rxDmaBuf[60] = {0};

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

HAL_StatusTypeDef lps_read_status(uint8_t addr, uint8_t *rxData, uint16_t size){
	char ADR_cmd[9] = {0};
	uint8_t repeat = 0;

	if ( (addr>=1 && addr<=32) /*&& (size == 54)*/ ){
		if ( addr < 10 )
			snprintf(ADR_cmd, sizeof(ADR_cmd), ":ADR0%1d;\r", addr);
		else
			snprintf(ADR_cmd, sizeof(ADR_cmd), ":ADR%2d;\r", addr);

		rs485_tx(ADR_cmd, sizeof(ADR_cmd), addr+1);

		lps_tx_w_respond(STT_cmd, sizeof(STT_cmd), rxData, size, 2);

		return HAL_OK;
	}

	return HAL_ERROR;
}


HAL_StatusTypeDef lps_ctrl_output(uint8_t addr, LpsOutputState_t state){
	char ADR_cmd[9] = {0};
	char lpsStatus[54] = {0};
	uint8_t repeat = 2;

	if ( addr>=1 && addr<=32 ){
		if ( addr < 10 )
			snprintf(ADR_cmd, sizeof(ADR_cmd), ":ADR0%1d;\r", addr);
		else
			snprintf(ADR_cmd, sizeof(ADR_cmd), ":ADR%2d;\r", addr);

		while(repeat){
			rs485_tx(ADR_cmd, sizeof(ADR_cmd), addr+1);

			if ( state == LPS_OUTPUT_ON ){
				rs485_tx(OUT1_cmd, sizeof(OUT1_cmd), addr+1);
				lps_read_status(addr, lpsStatus, sizeof(lpsStatus));
				if ( lpsStatus[LPS_STT_OS_REG_OUT_BIT] == '1' )
					return HAL_OK;
				else
					repeat--;
			}else{
				rs485_tx(OUT0_cmd, sizeof(OUT0_cmd), addr+1);
				lps_read_status(addr, lpsStatus, sizeof(lpsStatus));
				if ( lpsStatus[LPS_STT_OS_REG_OUT_BIT] == '0' )
					return HAL_OK;
				else
					repeat--;
			}
		}
	}

	return HAL_ERROR;
}

HAL_StatusTypeDef lps_find_connected(LpsStatusList_t *lpsList){
	HAL_StatusTypeDef res = HAL_ERROR;
	char ADR_cmd[9] = {0};
	char lpsMdlStr[30] = {0};

	lpsList->conNum = 0;
	memset(lpsList->conAddrsList, 0, sizeof(lpsList->conAddrsList));

	for (uint8_t addr=1; addr<=32; addr++){
		if ( addr < 10 )
			snprintf(ADR_cmd, sizeof(ADR_cmd), ":ADR0%1d;\r", addr);
		else
			snprintf(ADR_cmd, sizeof(ADR_cmd), ":ADR%2d;\r", addr);

		rs485_tx(ADR_cmd, sizeof(ADR_cmd), addr+1);

		memset(lpsMdlStr, 0, sizeof(lpsMdlStr));
		res = lps_tx_w_respond(MDL_cmd, sizeof(MDL_cmd), lpsMdlStr, strlen(lpsNames[0]), 2);

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

	return res;
}

HAL_StatusTypeDef lps_list_init(void){
	LpsStatusList_t *lpsList = &lpsStatusList;
	lps_find_connected(lpsList);
	// after finding the number of all connected lps let`s allocate mem for statusArray
	lpsList->statusArray = (LpsStatus_t *) calloc(lpsList->conNum, sizeof(LpsStatus_t));
	if (lpsList->statusArray == NULL)
		return HAL_ERROR;
	return HAL_OK;
}




