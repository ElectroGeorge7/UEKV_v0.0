/*
 * ts_spi.c
 *
 *  Created on: 30 окт. 2022 г.
 *      Author: George
 */

#ifndef TS_HARDWARE_TS_SPI_C_
#define TS_HARDWARE_TS_SPI_C_

#include "ts_spi.h"

#include "MAX6675.h"

SPI_HandleTypeDef hspi2;

static void SPI2_Init(void);

HAL_StatusTypeDef ts_spi_init(void){
	  GPIO_InitTypeDef GPIO_InitStruct = {0};

	  __HAL_RCC_GPIOC_CLK_ENABLE();

	  /*Configure GPIO pin Output Level */
	  HAL_GPIO_WritePin(GPIOC, TEMP_SENS_CS2_Pin | TEMP_SENS_CS1_Pin, GPIO_PIN_SET);	// disable both

	  /*Configure GPIO pins : TEMP_SENS_CS1_Pin TEMP_SENS_CS2_Pin*/
	  GPIO_InitStruct.Pin = TEMP_SENS_CS1_Pin|TEMP_SENS_CS2_Pin;
	  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	  GPIO_InitStruct.Pull = GPIO_NOPULL;
	  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
	  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

	  SPI2_Init();
}

float ts_check(uint8_t tsNum){
	float tsTemp = 0.0;

	if ( tsNum == 1 ){
		  HAL_GPIO_WritePin(GPIOC, TEMP_SENS_CS2_Pin, GPIO_PIN_RESET);
	} else if ( tsNum == 2 ){
		  HAL_GPIO_WritePin(GPIOC, TEMP_SENS_CS1_Pin, GPIO_PIN_RESET);
	}

	tsTemp = Max6675_Read_Temp();
	HAL_GPIO_WritePin(GPIOC, TEMP_SENS_CS2_Pin | TEMP_SENS_CS1_Pin, GPIO_PIN_SET);	// disable both

	if ( !((tsTemp > 0.0) && (tsTemp < 1024.0)) )
		tsTemp = 0.0;

	return tsTemp;
}

/**
  * @brief SPI2 Initialization Function
  * @param None
  * @retval None
  */
static void SPI2_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};

	__HAL_RCC_SPI2_CLK_ENABLE();

	__HAL_RCC_GPIOC_CLK_ENABLE();
	__HAL_RCC_GPIOB_CLK_ENABLE();
	/**SPI2 GPIO Configuration
	PC2     ------> SPI2_MISO
	PB10     ------> SPI2_SCK
	*/
	GPIO_InitStruct.Pin = GPIO_PIN_2;
	GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
	GPIO_InitStruct.Alternate = GPIO_AF5_SPI2;
	HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

	GPIO_InitStruct.Pin = GPIO_PIN_10;
	GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
	// SCK must be pulldown, because after receiving data SCK goes high
	// and it happens before thermo sensor turning off, so protocol is violated
	GPIO_InitStruct.Pull = GPIO_PULLDOWN;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
	GPIO_InitStruct.Alternate = GPIO_AF5_SPI2;
	HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /* SPI2 parameter configuration*/
  hspi2.Instance = SPI2;
  hspi2.Init.Mode = SPI_MODE_MASTER;
  hspi2.Init.Direction = SPI_DIRECTION_2LINES_RXONLY;
  hspi2.Init.DataSize = SPI_DATASIZE_16BIT;
  hspi2.Init.CLKPolarity = SPI_POLARITY_LOW;
  hspi2.Init.CLKPhase = SPI_PHASE_1EDGE;
  hspi2.Init.NSS = SPI_NSS_SOFT;
  hspi2.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_128;
  hspi2.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi2.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi2.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi2.Init.CRCPolynomial = 10;
  if (HAL_SPI_Init(&hspi2) != HAL_OK)
  {
    Error_Handler();
  }
}

#endif /* TS_HARDWARE_TS_SPI_C_ */
