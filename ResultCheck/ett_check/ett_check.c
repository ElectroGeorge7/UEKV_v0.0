/*
 * ett_check.c
 *
 *  Created on: 17 дек. 2022 г.
 *      Author: George
 */

#include "ett_check.h"

#include "main.h"
#include "cmsis_os2.h"
#include "terminal.h"
#include <string.h>

#include "rch_timer.h"

#define SPI_ETT_CS1_Pin 		GPIO_PIN_5
#define SPI_ETT_CS1_GPIO_Port 	GPIOB
#define SPI_ETT_CS2_Pin 		GPIO_PIN_12
#define SPI_ETT_CS2_GPIO_Port 	GPIOB
#define SPI_ETT_CS3_Pin 		GPIO_PIN_13
#define SPI_ETT_CS3_GPIO_Port 	GPIOB
#define SPI_ETT_CS4_Pin 		GPIO_PIN_14
#define SPI_ETT_CS4_GPIO_Port 	GPIOB

#define ETT_START_FLAG 0x5555
#define ETT_LETSGO_FLAG 0x5005

#define ETT_TIMEOUT 100

SPI_HandleTypeDef hspi1;

uint16_t ettBitMatrix[ETT_MATRIX_ROW_NUM] = {0};

static uint8_t confEttPcbNum = 0;

static void MX_SPI1_Init(void);

static void ett_sel_seg(uint8_t segNum);
static HAL_StatusTypeDef ett_send_flag(uint16_t flag, uint8_t segNum);
static HAL_StatusTypeDef ett_read_row_res(uint8_t *rowRes, uint8_t segNum);
static HAL_StatusTypeDef ett_seg_res_save(uint8_t segNum);


HAL_StatusTypeDef ett_check_init(TestConfig_t conf){
	HAL_StatusTypeDef res = HAL_OK;

	confEttPcbNum = conf.pcbNum;

	MX_SPI1_Init();

	switch (conf.resCheckMethod){
		case AVERAGE_RESULT:
			res |= rch_timer_init(1000 * conf.resCheckPeriod);
			res |= rch_timer_start();
			break;

		case SYNCHRO_RESULT:
		default:
			uartprintf("ett check incorrect method");
			break;
	}

	return res;
}

HAL_StatusTypeDef ett_check(uint16_t *resBitMatrix){
	HAL_StatusTypeDef res = HAL_OK;
	HAL_StatusTypeDef resTemp = HAL_OK;

	for (uint8_t segNum = 1; segNum <= confEttPcbNum; segNum++){
		ett_send_flag(ETT_START_FLAG, segNum);
		if ((resTemp = ett_send_flag(ETT_LETSGO_FLAG, segNum)) == HAL_OK){
			res |= resTemp;
			res |= ett_seg_res_save(segNum);
		}
	};

	if ( (res == HAL_OK) && (resBitMatrix != NULL) )
		memcpy(resBitMatrix, ettBitMatrix, sizeof(ettBitMatrix));

    return res;
}

void ett_res_clear(void){
	memset(ettBitMatrix, 0, sizeof(ettBitMatrix));
}

static void ett_delay(void){
	uint32_t timeout = 1000;
	while(timeout--);
}

static void ett_sel_seg(uint8_t segNum){
	switch(segNum){
	case 1:
		HAL_GPIO_WritePin(GPIOB, SPI_ETT_CS1_Pin | SPI_ETT_CS2_Pin | SPI_ETT_CS3_Pin | SPI_ETT_CS4_Pin, GPIO_PIN_SET);
		HAL_GPIO_WritePin(GPIOB, SPI_ETT_CS1_Pin, GPIO_PIN_RESET);
		ett_delay();
		break;
	case 2:
		HAL_GPIO_WritePin(GPIOB, SPI_ETT_CS1_Pin | SPI_ETT_CS2_Pin | SPI_ETT_CS3_Pin | SPI_ETT_CS4_Pin, GPIO_PIN_SET);
		HAL_GPIO_WritePin(GPIOB, SPI_ETT_CS2_Pin, GPIO_PIN_RESET);
		ett_delay();
		break;
	case 3:
		HAL_GPIO_WritePin(GPIOB, SPI_ETT_CS1_Pin | SPI_ETT_CS2_Pin | SPI_ETT_CS3_Pin | SPI_ETT_CS4_Pin, GPIO_PIN_SET);
		HAL_GPIO_WritePin(GPIOB, SPI_ETT_CS3_Pin, GPIO_PIN_RESET);
		ett_delay();
		break;
	case 4:
		HAL_GPIO_WritePin(GPIOB, SPI_ETT_CS1_Pin | SPI_ETT_CS2_Pin | SPI_ETT_CS3_Pin | SPI_ETT_CS4_Pin, GPIO_PIN_SET);
		HAL_GPIO_WritePin(GPIOB, SPI_ETT_CS4_Pin, GPIO_PIN_RESET);
		ett_delay();
		break;
	case 0:
	default:
		ett_delay();
		HAL_GPIO_WritePin(GPIOB, SPI_ETT_CS1_Pin | SPI_ETT_CS2_Pin | SPI_ETT_CS3_Pin | SPI_ETT_CS4_Pin, GPIO_PIN_SET);
		ett_delay();
	}
}

static HAL_StatusTypeDef ett_send_flag(uint16_t flag, uint8_t segNum){
	HAL_StatusTypeDef res = HAL_OK;
	uint16_t timeout = ETT_TIMEOUT;
	uint16_t flagResp = 0;

	while ((flagResp != flag+1) && timeout--) {
	  ett_sel_seg(segNum);
	  res = HAL_SPI_TransmitReceive(&hspi1, (uint8_t *)&flag, (uint8_t *)&flagResp,  1, 0xff);
	  ett_sel_seg(0);

	  if ( (res == HAL_OK) && (flagResp == flag+1) ) {
		  return HAL_OK;
	  }
	}

	return (res != HAL_OK) ? res : HAL_ERROR;
}

static HAL_StatusTypeDef ett_read_row_res(uint8_t *rowRes, uint8_t segNum){
	HAL_StatusTypeDef res = HAL_OK;

	ett_sel_seg(segNum);
	res = HAL_SPI_Receive(&hspi1, rowRes, 1, 0xfff);
	ett_sel_seg(0);

	return res;
}

static HAL_StatusTypeDef ett_seg_res_save(uint8_t segNum){
	HAL_StatusTypeDef res = HAL_OK;
	uint16_t rowRes = 0;
	uint16_t timeout = ETT_TIMEOUT;

	for (uint8_t rowNum = 0; rowNum < ETT_SEG_ROW_NUM; rowNum++){
		do{
			res = ett_read_row_res((uint8_t *)&rowRes, segNum);
		} while( ((rowRes >> 8) >= 0x50) && timeout-- );	// check that it`s not a flag

		if (timeout == 0)
			return (res != HAL_OK) ? res : HAL_ERROR;

		rowRes &= 0x7f;

		switch(segNum){
		case 1:
			ettBitMatrix[rowNum] |= rowRes;
			break;
		case 2:
			ettBitMatrix[rowNum+ETT_SEG_ROW_NUM] |= rowRes;
			break;
		case 3:
			ettBitMatrix[rowNum] |= rowRes << 7;
			break;
		case 4:
			ettBitMatrix[rowNum+ETT_SEG_ROW_NUM] |= rowRes << 7;
			break;
		}

	}

	return res;
}


/**
  * @brief SPI1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_SPI1_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

	/* Peripheral clock enable */
	__HAL_RCC_SPI1_CLK_ENABLE();

	__HAL_RCC_GPIOA_CLK_ENABLE();
	/**SPI1 GPIO Configuration
	PA5     ------> SPI1_SCK
	PA6     ------> SPI1_MISO
	PA7     ------> SPI1_MOSI
	*/
	GPIO_InitStruct.Pin = GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7;
	GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
	GPIO_InitStruct.Alternate = GPIO_AF5_SPI1;
	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  __HAL_RCC_GPIOB_CLK_ENABLE();
  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, SPI_ETT_CS1_Pin|SPI_ETT_CS2_Pin|SPI_ETT_CS3_Pin|SPI_ETT_CS4_Pin, GPIO_PIN_SET);

  /*Configure GPIO pins : SPI_ETT_CS2_Pin SPI_ETT_CS3_Pin SPI_ETT_CS4_Pin SPI_ETT_CS_Pin */
  GPIO_InitStruct.Pin = SPI_ETT_CS1_Pin|SPI_ETT_CS2_Pin|SPI_ETT_CS3_Pin|SPI_ETT_CS4_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /* SPI1 parameter configuration*/
  hspi1.Instance = SPI1;
  hspi1.Init.Mode = SPI_MODE_MASTER;
  hspi1.Init.Direction = SPI_DIRECTION_2LINES;
  hspi1.Init.DataSize = SPI_DATASIZE_16BIT;
  hspi1.Init.CLKPolarity = SPI_POLARITY_LOW;
  hspi1.Init.CLKPhase = SPI_PHASE_1EDGE;
  hspi1.Init.NSS = SPI_NSS_SOFT;
  hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_256;
  hspi1.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi1.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi1.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi1.Init.CRCPolynomial = 10;
  if (HAL_SPI_Init(&hspi1) != HAL_OK)
  {
    Error_Handler();
  }

}


/**
* @brief SPI MSP De-Initialization
* This function freeze the hardware resources used in this example
* @param hspi: SPI handle pointer
* @retval None
*/
void HAL_SPI_MspDeInit(SPI_HandleTypeDef* hspi)
{
  if(hspi->Instance==SPI1)
  {
    /* Peripheral clock disable */
    __HAL_RCC_SPI1_CLK_DISABLE();

    /**SPI1 GPIO Configuration
    PA5     ------> SPI1_SCK
    PA6     ------> SPI1_MISO
    PA7     ------> SPI1_MOSI
    */
    HAL_GPIO_DeInit(GPIOA, GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7);

  }
  else if(hspi->Instance==SPI2)
  {
    /* Peripheral clock disable */
    __HAL_RCC_SPI2_CLK_DISABLE();

    /**SPI2 GPIO Configuration
    PC2     ------> SPI2_MISO
    PB10     ------> SPI2_SCK
    */
    HAL_GPIO_DeInit(GPIOC, GPIO_PIN_2);

    HAL_GPIO_DeInit(GPIOB, GPIO_PIN_10);

  }

}
