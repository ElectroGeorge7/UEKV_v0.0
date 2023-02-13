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

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* Private defines -----------------------------------------------------------*/
#define LED_ERROR_Pin GPIO_PIN_12
#define LED_ERROR_GPIO_Port GPIOC
#define LED_PROCESS_Pin GPIO_PIN_13
#define LED_PROCESS_GPIO_Port GPIOC

#define BUTTON_UP_PRESS_EVENT 			0x01
#define BUTTON_DOWN_PRESS_EVENT 		0x02
#define BUTTON_RIGHT_PRESS_EVENT 		0x03
#define BUTTON_LEFT_PRESS_EVENT 		0x04
#define TERMINAL_СMD_RECEIVED_EVENT		0x05
#define TEST_CONFIG_SEARCH 				0x06
#define TEST_CONFIG_IS_FIND				0x07
#define TEST_CONFIG_IS_NOT				0x08
#define TEST_CONFIG_SEND				0x09
#define TEST_START						0x0a
#define TEST_FINISH						0x0b

#include "rtc_hardware.h"

typedef struct {
	char intVal;
	uint16_t fracVal;
} Current_t;

typedef struct {
	char intVal;
	uint16_t fracVal;
} Voltage_t;

typedef struct {
	uint32_t index;
	DataTime_t dataTime;
	uint16_t result[10];
	float temp[2];
	Current_t supplyCurrents[32];
	Voltage_t supplyVoltages[32];
} Log_t;

typedef struct {
	uint8_t index;
	uint8_t event;
	char eventStr[256];
} Event_t;


#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
