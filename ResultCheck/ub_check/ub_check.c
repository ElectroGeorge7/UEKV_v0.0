/*
 * pcf8575.c
 *
 *  Created on: Oct 1, 2022
 *      Author: George
 */

#include "ub_check.h"
#include <string.h>

#define PCF8575_READ_ADDR	0x41
#define PCF8575_WRITE_ADDR	0x40

#define UB_CHECK_TIMEOUT	0xffff
#define UB_CHECK_REPEAT_CNT	16

I2C_HandleTypeDef hi2c1;

uint16_t portExpPacket = 0;
uint8_t rowBitArray = 0;
uint8_t colBitArray = 0;

//uint8_t ubMatrix[8] = {0};

static void I2C1_Init(void);

HAL_StatusTypeDef ub_check_init(){
	I2C1_Init();
	HAL_I2C_Master_Transmit(&hi2c1, PCF8575_WRITE_ADDR, (uint8_t *)&portExpPacket, sizeof(portExpPacket), UB_CHECK_TIMEOUT);
	HAL_I2C_Master_Receive(&hi2c1, PCF8575_READ_ADDR, (uint8_t *)&portExpPacket, sizeof(portExpPacket), UB_CHECK_TIMEOUT);
	return HAL_OK;
}

HAL_StatusTypeDef ub_check(){
	uint8_t checkRepeatCnt = UB_CHECK_REPEAT_CNT;

	while (checkRepeatCnt--){
		HAL_I2C_Master_Receive(&hi2c1, PCF8575_READ_ADDR, (uint8_t *)&portExpPacket, sizeof(portExpPacket), UB_CHECK_TIMEOUT);

		colBitArray = (uint8_t)portExpPacket;
		rowBitArray = (uint8_t) (portExpPacket >> 8);

		for (uint8_t rowNum = 0; rowNum < UB_MATRIX_ROW_NUM; rowNum++){
			if ( rowBitArray & (1<<rowNum) ){
				ubMatrix[rowNum] |= colBitArray;
				break;
			}
		}
	}
  
  return HAL_OK;
}

HAL_StatusTypeDef ub_res_clear(void){
	memset(ubMatrix, 0, sizeof(ubMatrix));

	portExpPacket = 0;
	rowBitArray = 0;
	colBitArray = 0;

	return HAL_OK;
}

/**
  * @brief I2C1 Initialization Function
  * @param None
  * @retval None
  */
static void I2C1_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  __HAL_RCC_I2C1_CLK_ENABLE();

  hi2c1.Instance = I2C1;
  hi2c1.Init.ClockSpeed = 100000;
  hi2c1.Init.DutyCycle = I2C_DUTYCYCLE_2;
  hi2c1.Init.OwnAddress1 = 0;
  hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c1.Init.OwnAddress2 = 0;
  hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c1) != HAL_OK)
  {
    Error_Handler();
  }

  __HAL_RCC_GPIOB_CLK_ENABLE();
  /**I2C1 GPIO Configuration
  PB6     ------> I2C1_SCL
  PB7     ------> I2C1_SDA
  */
  GPIO_InitStruct.Pin = GPIO_PIN_6|GPIO_PIN_7;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_OD;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  GPIO_InitStruct.Alternate = GPIO_AF4_I2C1;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
}
