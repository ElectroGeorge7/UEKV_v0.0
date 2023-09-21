/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2022 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"

#define SOFTWARE_VER	"SW: v.1.1.1"
#define HARDWARE_VER	"HW: UEKV_V3"

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

#define BUTTON_UP_PRESS_EVENT 			0x01
#define BUTTON_DOWN_PRESS_EVENT 		0x02
#define BUTTON_RIGHT_PRESS_EVENT 		0x04
#define BUTTON_LEFT_PRESS_EVENT 		0x08
#define ACTIVITY_UPDATE_EVENT			0x10
#define TERMINAL_CMD_RECEIVED_EVENT		0x20
#define TEST_CONFIG_SEARCH				0x40
#define TEST_CONFIG_IS_FIND				0x80
#define TEST_CONFIG_IS_NOT				0x100
#define TEST_CONFIG_SEND				0x200
#define TEST_START						0x400
#define TEST_FINISH						0x800
#define TEST_LOG_SAVE					0x1000
#define TEST_LOG_DISPLAY				0x2000
#define TEST_LOG_PROCCESS_FINISHED		0x4000
#define LPS_LIST_UDATE_START			0x8000
#define LPS_LIST_UDATE_FINISHED			0x10000
#define LPS_FIND_CONNECTED_START		0x20000
#define LPS_FIND_CONNECTED_FINISHED		0x40000

#include "rtc_hardware.h"
#include "lps_task.h"

//typedef struct {
//	uint8_t intVal;
//	uint16_t fracVal;
//} Current_t;
//
//typedef struct {
//	uint8_t intVal;
//	uint16_t fracVal;
//} Voltage_t;


// typedef struct {
// 	uint8_t addr;
// 	char volStr[7];
// 	char curStr[7];
// 	char status;
// } LpsStatus_t;

typedef struct {
	uint32_t index;
	DataTime_t dataTime;
	uint16_t result[10];
	float temp[2];
	//Current_t supplyCurrents[32];
	//Voltage_t supplyVoltages[32];
	//char lpsState[54];
	LpsStatus_t *lpsStatusArray;
} Log_t;

typedef struct {
	uint8_t index;
	uint32_t event;
	char eventStr[256];
} Event_t;


#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
