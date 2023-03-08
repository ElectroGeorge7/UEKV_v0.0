/*
 * lps_control.c
 *
 *  Created on: 5 мар. 2023 г.
 *      Author: George
 */

#include "lps_control.h"
#include "rs485_hardware.h"

#include <string.h>
#include <stdio.h>

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

#define LPS_STT_OS_REG_OUT_BIT	33

static uint8_t lpsNum = 0;
static uint8_t lpsAddrList[32] = {0};

// обязательно нужно перепроверять принятые данные, и попытаться повторно их прочитать, если с первого раза не получилось
                // заметил такую проблему, чем дальше ЛБП в цепочке от ЭКВ, тем большее количество раз нужно отправлять одну и ту же команду
                // т.е. если ЛБП второй в цепочке, то одну и ту же команду нужно отправить 2 раза, чтобы она гарантировано выполнилась, если 3 в цепочке- соответственно 3 раза, и т.д.
// кстати, возможно ещё дело в очень длинном кабеле, третий езернет кабель был значительно длинее первых двух.

// по очереди отправлять на адрес i=(1-32) команду MDL
// и сравнить с шаблонами
// записать все активные адреса в массив
// затем создать функции по чтению всех лбп
HAL_StatusTypeDef lps_find_connected(void){
	HAL_StatusTypeDef res = HAL_ERROR;
	char ADR_cmd[9] = {0};
	char lpsMdlStr[30] = {0};

	lpsNum = 0;
	memset(lpsAddrList, 0, sizeof(lpsAddrList));

	for (uint8_t addr=1; addr<=32; addr++){
		if ( addr < 10 )
			snprintf(ADR_cmd, sizeof(ADR_cmd), ":ADR0%1d;\r", addr);
		else
			snprintf(ADR_cmd, sizeof(ADR_cmd), ":ADR%2d;\r", addr);

		rs485_transmit(ADR_cmd, sizeof(ADR_cmd), addr+1);

		memset(lpsMdlStr, 0, sizeof(lpsMdlStr));
		res = rs485_transmit_w_respond(MDL_cmd, sizeof(MDL_cmd), lpsMdlStr, strlen(lpsNames[0]), 2);

		for (uint8_t i = 0; i < LPS_NAMES_NUM; i++){
			if ( strncmp(lpsMdlStr, lpsNames[i], strlen(lpsNames[i])) == 0 ){
				lpsNum++;
				lpsAddrList[addr-1] = 0xff;
				// toggle lps output to check the control access
                lps_ctrl_output(addr, LPS_OUTPUT_ON);
                HAL_Delay(3000);
                lps_ctrl_output(addr, LPS_OUTPUT_OFF);
			}
		}
	}

	return res;
}

uint8_t lps_get_connected_quantity(void){
	return lpsNum;
}

HAL_StatusTypeDef lps_read_status(uint8_t addr, uint8_t *rxData, uint16_t size){
	char ADR_cmd[9] = {0};
	uint8_t repeat = 0;

	if ( (addr>=1 && addr<=32) /*&& (size == 54)*/ ){
		if ( addr < 10 )
			snprintf(ADR_cmd, sizeof(ADR_cmd), ":ADR0%1d;\r", addr);
		else
			snprintf(ADR_cmd, sizeof(ADR_cmd), ":ADR%2d;\r", addr);

		rs485_transmit(ADR_cmd, sizeof(ADR_cmd), addr+1);

		rs485_transmit_w_respond(STT_cmd, sizeof(STT_cmd), rxData, size, addr+1);

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
			rs485_transmit(ADR_cmd, sizeof(ADR_cmd), addr+1);

			if ( state == LPS_OUTPUT_ON ){
				rs485_transmit(OUT1_cmd, sizeof(OUT1_cmd), addr+1);
				lps_read_status(addr, lpsStatus, sizeof(lpsStatus));
				if ( lpsStatus[LPS_STT_OS_REG_OUT_BIT] == '1' )
					return HAL_OK;
				else
					repeat--;
			}else{
				rs485_transmit(OUT0_cmd, sizeof(OUT0_cmd), addr+1);
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
