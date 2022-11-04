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

//#include "lm_timer.h"

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* Private defines -----------------------------------------------------------*/
#define LED_ERROR_Pin GPIO_PIN_12
#define LED_ERROR_GPIO_Port GPIOC

#define RS485_nRE_Pin GPIO_PIN_2
#define RS485_nRE_GPIO_Port GPIOA
#define RS485_DE_Pin GPIO_PIN_3
#define RS485_DE_GPIO_Port GPIOA
#define SPI_ETT_CS2_Pin GPIO_PIN_12
#define SPI_ETT_CS2_GPIO_Port GPIOB
#define SPI_ETT_CS3_Pin GPIO_PIN_13
#define SPI_ETT_CS3_GPIO_Port GPIOB
#define SPI_ETT_CS4_Pin GPIO_PIN_15
#define SPI_ETT_CS4_GPIO_Port GPIOB
#define LED_PROCESS_Pin GPIO_PIN_13
#define LED_PROCESS_GPIO_Port GPIOC

#define SPI_ETT_CS_Pin GPIO_PIN_5
#define SPI_ETT_CS_GPIO_Port GPIOB
/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
