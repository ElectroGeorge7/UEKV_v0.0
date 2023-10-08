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

#define SOFTWARE_VER	"SW: v.1.2.1"
#define HARDWARE_VER	"HW: UEKV_V3"

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* event_groups.h, 116: each event group has 24 usable bits (bit 0 to bit 23). */
#define BUTTON_UP_PRESS_EVENT 			((uint32_t)(1 << 0))
#define BUTTON_DOWN_PRESS_EVENT 		((uint32_t)(1 << 1))
#define BUTTON_RIGHT_PRESS_EVENT 		((uint32_t)(1 << 2))
#define BUTTON_LEFT_PRESS_EVENT 		((uint32_t)(1 << 3))
#define ACTIVITY_UPDATE_EVENT			((uint32_t)(1 << 4))
#define TERMINAL_CMD_RECEIVED_EVENT		((uint32_t)(1 << 5))
#define TEST_CONFIG_SEARCH				((uint32_t)(1 << 6))
#define TEST_CONFIG_IS_FIND				((uint32_t)(1 << 7))
#define TEST_CONFIG_IS_NOT				((uint32_t)(1 << 8))
#define TEST_CONFIG_SEND				((uint32_t)(1 << 9))
#define TEST_START						((uint32_t)(1 << 10))
#define TEST_FINISH						((uint32_t)(1 << 11))
#define TEST_LOG_SAVE					((uint32_t)(1 << 12))
#define TEST_LOG_DISPLAY				((uint32_t)(1 << 13))
#define TEST_LOG_PROCCESS_FINISHED		((uint32_t)(1 << 14))
#define LPS_LIST_UPDATE_START			((uint32_t)(1 << 15))
#define LPS_LIST_UPDATE_FINISHED		((uint32_t)(1 << 16))
#define LPS_FIND_CONNECTED_START		((uint32_t)(1 << 17))
#define LPS_FIND_CONNECTED_FINISHED		((uint32_t)(1 << 18))
#define LPS_CONFIG_SEARCH				((uint32_t)(1 << 19))
#define LPS_CONFIG_FINISH				((uint32_t)(1 << 20))
#define LPS_CONFIG_START				((uint32_t)(1 << 21))
#define LPS_CONFIG_DONE					((uint32_t)(1 << 22))
#define LPS_CONFIG_ERROR				((uint32_t)(1 << 23))

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
