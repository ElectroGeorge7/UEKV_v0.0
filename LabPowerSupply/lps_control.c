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

uint8_t MDL_cmd[] = ":MDL?;\r";
uint8_t REV_cmd[] = ":REV?;\r";
uint8_t RMT0_cmd[] = ":RMT0;\r";
uint8_t RMT1_cmd[] = ":RMT1;\r";
uint8_t RMT2_cmd[] = ":RMT2;\r";
uint8_t OUT1_cmd[] = ":OUT1;\r";
uint8_t OUT0_cmd[] = ":OUT0;\r";
uint8_t STT_cmd[] = ":STT?;\r";

static uint8_t lpsNum = 0;
static uint8_t lpsAddrOrder[32] = {0};
char lpsMdlStr[30] = {0};

// обязательно нужно перепроверять принятые данные, и попытаться повторно их прочитать, если с первого раза не получилось
                // заметил такую проблему, чем дальше ЛБП в цепочке от ЭКВ, тем большее количество раз нужно отправлять одну и ту же команду
                // т.е. если ЛБП второй в цепочке, то одну и ту же команду нужно отправить 2 раза, чтобы она гарантировано выполнилась, если 3 в цепочке- соответственно 3 раза, и т.д.
// кстати, возможно ещё дело в очень длинном кабеле, третий езернет кабель был значительно длинее первых двух.

HAL_StatusTypeDef lps_find_all(void){
	HAL_StatusTypeDef res = HAL_ERROR;
	char ADR_cmd[9] = {0};
	// по очереди отправлять на адрес i=(1-32) команду MDL
	// и сравнить с шаблонами
	// записать все активные адреса в массив
	// затем создать функции по чтению всех лбп
	//char lpsMdlStr[30] = {0};
	for (uint8_t addr=1; addr<=32; addr++){
		if ( addr < 10 )
			snprintf(ADR_cmd, sizeof(ADR_cmd), ":ADR0%1d;\r", addr);
		else
			snprintf(ADR_cmd, sizeof(ADR_cmd), ":ADR%2d;\r", addr);

		rs485_transmit(ADR_cmd, sizeof(ADR_cmd), addr+1);

		memset(lpsMdlStr, 0, sizeof(lpsMdlStr));
		res = rs485_transmit_w_respond(MDL_cmd, sizeof(MDL_cmd), lpsMdlStr, sizeof("Nemic-Lambda ZUP(60V-7A)\r\n"), 4);
		if ( !strncmp(lpsMdlStr, "Nemic-Lambda ZUP(6V-33A)\r\n", sizeof("Nemic-Lambda ZUP(60V-7A)\r\n")) ){
			lpsNum++;
			lpsAddrOrder[addr] = 0xff;
		}
	}

	return res;
}

uint8_t lps_get_quantity(void){
	return lpsNum;
}

HAL_StatusTypeDef lps_read_status(uint8_t addr, uint8_t *rxData, uint16_t size){
	char ADR_cmd[9] = {0};
	uint8_t repeat = 0;

	if ( (addr>=1 && addr<=32) && (size == 54) ){
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

	if ( addr>=1 && addr<=32 ){
		if ( addr < 10 )
			snprintf(ADR_cmd, sizeof(ADR_cmd), ":ADR0%1d;\r", addr);
		else
			snprintf(ADR_cmd, sizeof(ADR_cmd), ":ADR%2d;\r", addr);

		rs485_transmit(ADR_cmd, sizeof(ADR_cmd), addr+1);

		if ( state == LPS_OUTPUT_ON )
			rs485_transmit(OUT1_cmd, sizeof(OUT1_cmd), addr+1);
		else
			rs485_transmit(OUT0_cmd, sizeof(OUT0_cmd), addr+1);

		return HAL_OK;
	}

	return HAL_ERROR;
}
