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
extern osMessageQueueId_t eventQueueHandler;

#define LPS_MIN_ADDR	1
#define LPS_MAX_ADDR	32

#define LPS_RESP_MAX_WAIT_TIME	1000

#define LPS_STT_RESPOND_SIZE 	54

typedef enum LpsParamType{
	LPS_VOL,
	LPS_CUR
} LpsParamType_t;

typedef enum LpsParamFormat{
	LPS_PARAM_FORMAT_0,		//xx.xx
	LPS_PARAM_FORMAT_1,		//x.xxx
	LPS_PARAM_FORMAT_2		//xx.xxx
//	LPS_FORMAT_3			//xxx.xx - only for ZUP6-132, this format isn`t used
} LpsParamFormat_t;


#define LPS_ZUP60_7_VOL_FORMAT			LPS_PARAM_FORMAT_0
#define LPS_ZUP60_7_CUR_FORMAT			LPS_PARAM_FORMAT_1
#define LPS_ZUP60_7_STT_AV_START_BIT	2
#define LPS_ZUP60_7_STT_SV_START_BIT	LPS_ZUP60_7_STT_AV_START_BIT + 2 + ((LPS_ZUP60_7_VOL_FORMAT == LPS_PARAM_FORMAT_2) ? 6 : 5)
#define LPS_ZUP60_7_STT_AA_START_BIT	LPS_ZUP60_7_STT_SV_START_BIT + 2 + ((LPS_ZUP60_7_VOL_FORMAT == LPS_PARAM_FORMAT_2) ? 6 : 5)
#define LPS_ZUP60_7_STT_SA_START_BIT	LPS_ZUP60_7_STT_AA_START_BIT + 2 + ((LPS_ZUP60_7_CUR_FORMAT == LPS_PARAM_FORMAT_2) ? 6 : 5)
#define LPS_ZUP60_7_STT_OS_REG_OUT_BIT	33

#define LPS_ZUP10_40_VOL_FORMAT			LPS_PARAM_FORMAT_2
#define LPS_ZUP10_40_CUR_FORMAT			LPS_PARAM_FORMAT_0
#define LPS_ZUP10_40_STT_AV_START_BIT	2
#define LPS_ZUP10_40_STT_SV_START_BIT	LPS_ZUP10_40_STT_AV_START_BIT + 2 + ((LPS_ZUP10_40_VOL_FORMAT == LPS_PARAM_FORMAT_2) ? 6 : 5)
#define LPS_ZUP10_40_STT_AA_START_BIT	LPS_ZUP10_40_STT_SV_START_BIT + 2 + ((LPS_ZUP10_40_VOL_FORMAT == LPS_PARAM_FORMAT_2) ? 6 : 5)
#define LPS_ZUP10_40_STT_SA_START_BIT	LPS_ZUP10_40_STT_AA_START_BIT + 2 + ((LPS_ZUP10_40_CUR_FORMAT == LPS_PARAM_FORMAT_2) ? 6 : 5)


#define LPS_ZUP10_40_STT_AA_START_BIT	18
#define LPS_ZUP10_40_STT_OS_REG_OUT_BIT	33

#define LPS_NAMES_NUM	2
static const char *lpsNames[LPS_NAMES_NUM] = {
		"Nemic-Lambda ZUP(60V-7A)\r\n",
		"Nemic-Lambda ZUP(10V-40A)\r"
		/// @todo add the second type of lps
};

const char MDL_cmd[] = ":MDL?;\r";
const char REV_cmd[] = ":REV?;\r";
const char RMT0_cmd[] = ":RMT0;\r";
const char RMT1_cmd[] = ":RMT1;\r";
const char RMT2_cmd[] = ":RMT2;\r";
const char OUT1_cmd[] = ":OUT1;\r";
const char OUT0_cmd[] = ":OUT0;\r";
const char STT_cmd[] = ":STT?;\r";
const char VOL_cmd[] = ":VOL;\r";
const char CUR_cmd[] = ":CUR;\r";

const uint8_t VOL1_cmd[] = ":VOL06.000;\r";
const uint8_t VOL2_cmd[] = ":VOL7.300;\r";
const uint8_t VOL3_cmd[] = ":VOL08.50;\r";
const uint8_t CUR1_cmd[] = ":CUR01.000;\r";
const uint8_t CUR2_cmd[] = ":CUR1.200;\r";
const uint8_t CUR3_cmd[] = ":CUR01.30;\r";

typedef enum LpsOutputState{
	LPS_OUTPUT_OFF,
	LPS_OUTPUT_ON,
} LpsOutputState_t;

typedef struct{
	uint8_t addr;
	uint8_t type;
} LpsAddrType_t;

typedef struct {
	uint8_t updateReadyFlag;
	uint8_t conNum;
	LpsAddrType_t conAddrsList[32];
	LpsStatus_t *statusArray;
} LpsStatusList_t;

static LpsStatusList_t lpsStatusList = {0};



static HAL_StatusTypeDef lps_tx_w_respond(uint8_t *cmd, uint16_t txSize, uint8_t *respond, uint16_t rxSize, uint8_t repeat);
static HAL_StatusTypeDef lps_read_status(uint8_t addr, uint8_t *rxData, uint16_t size);
static HAL_StatusTypeDef lps_ctrl_output(uint8_t addr, LpsOutputState_t state);
static HAL_StatusTypeDef lps_find_connected(LpsStatusList_t *lpsList);
static HAL_StatusTypeDef lps_list_init(void);
static HAL_StatusTypeDef lps_conf_set(LpsStatus_t *lpsConfig);
static HAL_StatusTypeDef lps_param_set(LpsStatus_t *lpsConfig, LpsParamType_t paramType, LpsParamFormat_t format);
static void lps_param_cmd_make(char *cmdBuf, const char *cmdStr, char *paramStr, LpsParamFormat_t format);

void lps_respond_handler(void){
	osSemaphoreRelease(lpsRespondSem);
}

void LpsTask(void *argument){
	HAL_StatusTypeDef res = HAL_ERROR;
	char lpsStatusBuf[LPS_STT_RESPOND_SIZE] = {0};
	LpsStatus_t lpsConfig;
	LpsStatus_t *pStatusArray = NULL;
	uint32_t osEventFlag = 0;
	Event_t msg = {0};

	// register the lps_respond_handler() as interrupt handler
	rs485_init();
	rs485_reg_respond_handler(lps_respond_handler);

	osEventFlag = osEventFlagsWait(testEvents, LPS_FIND_CONNECTED_START | LPS_CONFIG_START, osFlagsWaitAny, osWaitForever);
	if ( osEventFlag & LPS_FIND_CONNECTED_START ){

		lps_list_init();
		///////////убрать после отладки
		//lps_ctrl_output(1, LPS_OUTPUT_ON);
		/////////////////////////////////
		osEventFlagsSet(testEvents, LPS_FIND_CONNECTED_FINISHED);
	} else if( osEventFlag & LPS_CONFIG_START ){
		// set flag to handle it in the loop
		osEventFlagsSet(testEvents, LPS_CONFIG_START);
	}


	for(;;){

		osEventFlag = osEventFlagsWait(testEvents, LPS_FIND_CONNECTED_START | LPS_LIST_UPDATE_START | LPS_CONFIG_START, osFlagsNoClear, osWaitForever);

		if ( osEventFlag & LPS_FIND_CONNECTED_START ){

			lps_list_init();

			osEventFlagsClear(testEvents, LPS_FIND_CONNECTED_START);
			osEventFlagsSet(testEvents, LPS_FIND_CONNECTED_FINISHED);

		} else if ( osEventFlag & LPS_LIST_UPDATE_START ){


			lpsStatusList.updateReadyFlag = 0;
			memset(lpsStatusList.statusArray, 0, lpsStatusList.conNum * sizeof(LpsStatus_t));

			for (uint8_t i = 0; i < lpsStatusList.conNum; i++ ){
				res = lps_read_status(lpsStatusList.conAddrsList[i].addr, lpsStatusBuf, LPS_STT_RESPOND_SIZE);
				if ( res == HAL_OK ){
					pStatusArray = &(lpsStatusList.statusArray[i]);
					pStatusArray->addr = lpsStatusList.conAddrsList[i].addr;
					switch(lpsStatusList.conAddrsList[i].type){
					case 0:
						memcpy(pStatusArray->volStr, lpsStatusBuf+LPS_ZUP60_7_STT_AV_START_BIT, 7-2);
						memcpy(pStatusArray->curStr, lpsStatusBuf+LPS_ZUP60_7_STT_AA_START_BIT, 7-2);
						break;
					case 1:
						memcpy(pStatusArray->volStr, lpsStatusBuf+LPS_ZUP10_40_STT_AV_START_BIT, 7-2);
						memcpy(pStatusArray->curStr, lpsStatusBuf+LPS_ZUP10_40_STT_AA_START_BIT, 7-2);
						break;
					default:
						memcpy(pStatusArray->volStr, lpsStatusBuf+LPS_ZUP60_7_STT_AV_START_BIT, 7-2);
						memcpy(pStatusArray->curStr, lpsStatusBuf+LPS_ZUP60_7_STT_AA_START_BIT, 7-2);
					}

				}
			}

			lpsStatusList.updateReadyFlag = 1;

			osEventFlagsClear(testEvents, LPS_LIST_UPDATE_START);
			//osEventFlagsSet(testEvents, LPS_LIST_UPDATE_FINISHED);

		} else if( osEventFlag & LPS_CONFIG_START ){

			osEventFlagsClear(testEvents, LPS_CONFIG_START);

			if( osMessageQueueGet(eventQueueHandler, &msg, NULL, osWaitForever) == osOK ){
				if ( msg.event == LPS_CONFIG_START ){
					memcpy((uint8_t *)&lpsConfig, msg.eventStr, sizeof(LpsStatus_t));
					if ( lps_conf_set(&lpsConfig) == HAL_OK ){
						osEventFlagsSet(testEvents, LPS_CONFIG_DONE);
					}else{
						osEventFlagsSet(testEvents, LPS_CONFIG_ERROR);
					}
				}
			}
		}

		osThreadYield();
		//osDelay(1);
	}
}

uint8_t lps_get_connected_num(void){
	return lpsStatusList.conNum;
}

uint8_t lps_get_update_ready_flag(void){
	return lpsStatusList.updateReadyFlag;
}

LpsStatus_t *lps_list_get(void){
	return lpsStatusList.statusArray;
}

static HAL_StatusTypeDef lps_conf_set(LpsStatus_t *lpsConfig){
	HAL_StatusTypeDef res = HAL_ERROR;
	LpsParamFormat_t format = LPS_PARAM_FORMAT_0;

	do{
		res = lps_param_set(lpsConfig, LPS_VOL, format);
	}while( (res != HAL_OK) && (++format <= LPS_PARAM_FORMAT_2) );

	if (res != HAL_OK)
		return HAL_ERROR;

	res = HAL_ERROR;
	do{
		res = lps_param_set(lpsConfig, LPS_CUR, format);
	}while( (res != HAL_OK) && (++format <= LPS_PARAM_FORMAT_2) );

	if (res != HAL_OK)
		return HAL_ERROR;

	return HAL_OK;
}

static HAL_StatusTypeDef lps_tx_w_respond(uint8_t *cmd, uint16_t txSize, uint8_t *respond, uint16_t rxSize, uint8_t repeat){
	HAL_StatusTypeDef res = HAL_ERROR;
	osStatus_t osRes = osError;
	char rxDmaBuf[60] = {0};

	while(repeat){

		res = rs485_tx(cmd, txSize, 1, 0);
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

	res = HAL_ERROR;
	return res;
}

static HAL_StatusTypeDef lps_read_status(uint8_t addr, uint8_t *rxData, uint16_t size){
	char ADR_cmd[9] = {0};
	uint8_t repeat = 0;

	if ( (addr>=1 && addr<=32) /*&& (size == 54)*/ ){
		if ( addr < 10 )
			snprintf(ADR_cmd, sizeof(ADR_cmd), ":ADR0%1d;\r", addr);
		else
			snprintf(ADR_cmd, sizeof(ADR_cmd), ":ADR%2d;\r", addr);

		rs485_tx(ADR_cmd, sizeof(ADR_cmd), addr+1, 10);

		lps_tx_w_respond(STT_cmd, sizeof(STT_cmd), rxData, size, 2);

		return HAL_OK;
	}

	return HAL_ERROR;
}

static HAL_StatusTypeDef lps_ctrl_output(uint8_t addr, LpsOutputState_t state){
	char ADR_cmd[9] = {0};
	char lpsStatus[54] = {0};
	uint8_t repeat = 2;

	if ( addr>=1 && addr<=32 ){
		if ( addr < 10 )
			snprintf(ADR_cmd, sizeof(ADR_cmd), ":ADR0%1d;\r", addr);
		else
			snprintf(ADR_cmd, sizeof(ADR_cmd), ":ADR%2d;\r", addr);

		while(repeat){
			rs485_tx(ADR_cmd, sizeof(ADR_cmd), addr+1, 10);

			if ( state == LPS_OUTPUT_ON ){
				rs485_tx(OUT1_cmd, sizeof(OUT1_cmd), addr+1, 10);
				lps_read_status(addr, lpsStatus, sizeof(lpsStatus));
				if ( lpsStatus[LPS_ZUP60_7_STT_OS_REG_OUT_BIT] == '1' )
					return HAL_OK;
				else
					repeat--;
			}else{
				rs485_tx(OUT0_cmd, sizeof(OUT0_cmd), addr+1, 10);
				lps_read_status(addr, lpsStatus, sizeof(lpsStatus));
				if ( lpsStatus[LPS_ZUP60_7_STT_OS_REG_OUT_BIT] == '0' )
					return HAL_OK;
				else
					repeat--;
			}
		}
	}

	return HAL_ERROR;
}

static HAL_StatusTypeDef lps_find_connected(LpsStatusList_t *lpsList){
	HAL_StatusTypeDef res = HAL_ERROR;
	char ADR_cmd[9] = {0};
	char lpsMdlStr[30] = {0};

	lpsList->conNum = 0;
	memset(lpsList->conAddrsList, 0, 32 * sizeof(LpsAddrType_t));

	for (uint8_t addr=1; addr<=32; addr++){
		if ( addr < 10 )
			snprintf(ADR_cmd, sizeof(ADR_cmd), ":ADR0%1d;\r", addr);
		else
			snprintf(ADR_cmd, sizeof(ADR_cmd), ":ADR%2d;\r", addr);

		rs485_tx(ADR_cmd, sizeof(ADR_cmd), addr+1, 10);

		memset(lpsMdlStr, 0, sizeof(lpsMdlStr));
		res = lps_tx_w_respond(MDL_cmd, sizeof(MDL_cmd), lpsMdlStr, strlen(lpsNames[0]), 2);

		for (uint8_t i = 0; i < LPS_NAMES_NUM; i++){
			if ( strncmp(lpsMdlStr, lpsNames[i], strlen(lpsNames[i])) == 0 ){
				lpsList->conAddrsList[lpsList->conNum].addr = addr;
				lpsList->conAddrsList[lpsList->conNum].type = i;
				lpsList->conNum++;
				// toggle lps output to check the control access
                lps_ctrl_output(addr, LPS_OUTPUT_ON);
                HAL_Delay(3000);
                lps_ctrl_output(addr, LPS_OUTPUT_OFF);
			}
		}
	}

	return res;
}

static HAL_StatusTypeDef lps_list_init(void){
	LpsStatusList_t *lpsList = &lpsStatusList;
	lps_find_connected(lpsList);
	// after finding the number of all connected lps let`s allocate mem for statusArray
	lpsList->statusArray = (LpsStatus_t *) calloc(lpsList->conNum, sizeof(LpsStatus_t));
	if (lpsList->statusArray == NULL)
		return HAL_ERROR;
	return HAL_OK;
}

static void lps_param_cmd_make(char *cmdBuf, const char *cmdStr, char *paramStr, LpsParamFormat_t format){
	memset(cmdBuf, 0, sizeof(cmdBuf));
	memcpy(cmdBuf, cmdStr, 4);				// :VOL

	switch( format ){
	case LPS_PARAM_FORMAT_0:
		memcpy(cmdBuf+4, paramStr, 5); 		// :VOLxx.xx
		memcpy(cmdBuf+9, cmdStr+4, 2);		// :VOLxx.xx;\r
		break;
	case LPS_PARAM_FORMAT_1:
		memcpy(cmdBuf+4, paramStr+1, 5); 	// :VOLx.xxx
		memcpy(cmdBuf+9, cmdStr+4, 2);		// :VOLx.xxx;\r
		break;
	case LPS_PARAM_FORMAT_2: default:
		memcpy(cmdBuf+4, paramStr, 6); 		// :VOLxx.xxx
		memcpy(cmdBuf+10, cmdStr+4, 2);		// :VOLxx.xxx;\r
		break;
	}
}

static HAL_StatusTypeDef lps_param_set(LpsStatus_t *lpsConfig, LpsParamType_t paramType, LpsParamFormat_t format){
	char ADR_cmd[9] = {0};
	char cmd_buf[13] = {0};
	char param_buf[7] = {0};
	char lpsStatus[54] = {0};
	char *paramStr = (paramType == LPS_VOL) ? lpsConfig->volStr : lpsConfig->curStr;

	float confParam = 0.0;
	float setParam = 0.0;
	char *svPos = 0;

	uint8_t repeat = 1;

	uint8_t addr = lpsConfig->addr;
	if ( addr>=1 && addr<=32 ){
		if ( addr < 10 )
			snprintf(ADR_cmd, sizeof(ADR_cmd), ":ADR0%1d;\r", addr);
		else
			snprintf(ADR_cmd, sizeof(ADR_cmd), ":ADR%2d;\r", addr);

		sscanf(paramStr, "%f", &confParam);
		lps_param_cmd_make(cmd_buf, (paramType == LPS_VOL) ? VOL_cmd : CUR_cmd, paramStr, format);

		do{
			rs485_tx(ADR_cmd, sizeof(ADR_cmd), addr+1, 10);
			rs485_tx(cmd_buf, strlen(cmd_buf)+1, addr+1, 10);
			lps_read_status(addr, lpsStatus, sizeof(lpsStatus));
			// check what was set in real
			svPos = strstr(lpsStatus, (paramType == LPS_VOL) ? "SV" : "SA");
			memset(param_buf, 0 , sizeof(param_buf));
			memcpy(param_buf, svPos + 2, (format == LPS_PARAM_FORMAT_2) ? 6 : 5);
			sscanf(param_buf, "%f", &setParam);

			if ( ((confParam > setParam) ? confParam - setParam : setParam - confParam) < 0.1 ){
				return HAL_OK;
			}
		} while(repeat--);
	}

	return HAL_ERROR;
}
