/*
 * rs485_hardware.c
 *
 *  Created on: 5 нояб. 2022 г.
 *      Author: George
 */

#include "rs485_hardware.h"
#include <string.h>

#define RS485_nRE_Pin GPIO_PIN_2
#define RS485_nRE_GPIO_Port GPIOA
#define RS485_DE_Pin GPIO_PIN_3
#define RS485_DE_GPIO_Port GPIOA

#define TIMEOUT 0xfff

UART_HandleTypeDef huart4;
DMA_HandleTypeDef hdma_uart4_rx;

static Rs485RespondHandler_t respondHandler = NULL;

static void UART4_Init(void);
static void DMA1_Init(void);

static uint8_t rxDmaCpltFlag = 0;
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	if (huart->Instance == UART4){
		respondHandler();
	}
}

HAL_StatusTypeDef rs485_receive(uint8_t *rxData, uint16_t size, uint8_t repeat){
	HAL_StatusTypeDef res = HAL_ERROR;
	while(repeat--){
		res = HAL_UART_Receive(&huart4, rxData, size, TIMEOUT);
	}
	return res;
}

HAL_StatusTypeDef rs485_transmit(uint8_t *cmd, uint16_t size, uint8_t repeat){
	HAL_StatusTypeDef res = HAL_ERROR;
	while(repeat--){
		res = HAL_UART_Transmit(&huart4, cmd, size, TIMEOUT);
		if (!repeat)
			HAL_Delay(10);
	}
	return res;
}

HAL_StatusTypeDef rs485_transmit_w_respond(uint8_t *cmd, uint16_t txSize, uint8_t *respond, uint16_t rxSize, uint8_t repeat){
	HAL_StatusTypeDef res = HAL_ERROR;
	uint8_t repVal = repeat;
	uint32_t timeout = 100;
	char rxDmaBuf[60] = {0};

	while(repeat){
		res = HAL_UART_Transmit(&huart4, cmd, txSize, TIMEOUT);
		res = HAL_UART_Receive_DMA(&huart4, rxDmaBuf, rxSize);
		while( !rxDmaCpltFlag && timeout-- ){
			HAL_Delay(1);
		};

		HAL_UART_DMAStop(&huart4);

		if (rxDmaCpltFlag){
			rxDmaCpltFlag = repeat = 0;
			memcpy(respond, rxDmaBuf, rxSize);
			return HAL_OK;
		}else{
			repeat--;
			timeout = 100;
		}
	}

	return res;
}

HAL_StatusTypeDef rs485_init(void){
	GPIO_InitTypeDef GPIO_InitStruct = {0};

	__HAL_RCC_GPIOA_CLK_ENABLE();

	/*Configure GPIO pin Output Level */
	HAL_GPIO_WritePin(GPIOA, RS485_DE_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOA, RS485_nRE_Pin, GPIO_PIN_SET);

	/*Configure GPIO pins : RS485_nRE_Pin RS485_DE_Pin */
	GPIO_InitStruct.Pin = RS485_nRE_Pin|RS485_DE_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

	UART4_Init();
	DMA1_Init();

	HAL_GPIO_WritePin(GPIOA, RS485_DE_Pin, GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOA, RS485_nRE_Pin, GPIO_PIN_RESET);

	return HAL_OK;
}

void rs485_reg_respond_handler(Rs485RespondHandler_t hFunc){
	respondHandler = hFunc;
}

/**
  * @brief UART4 Initialization Function
  * @param None
  * @retval None
  */
static void UART4_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* Peripheral clock enable */
  __HAL_RCC_UART4_CLK_ENABLE();

  __HAL_RCC_GPIOA_CLK_ENABLE();
  /**UART4 GPIO Configuration
  PA0-WKUP     ------> UART4_TX
  PA1     ------> UART4_RX
  */
  GPIO_InitStruct.Pin = GPIO_PIN_0|GPIO_PIN_1;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  GPIO_InitStruct.Alternate = GPIO_AF8_UART4;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  huart4.Instance = UART4;
  huart4.Init.BaudRate = 9600;
  huart4.Init.WordLength = UART_WORDLENGTH_8B;
  huart4.Init.StopBits = UART_STOPBITS_1;
  huart4.Init.Parity = UART_PARITY_NONE;
  huart4.Init.Mode = UART_MODE_TX_RX;
  huart4.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart4.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart4) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * Enable DMA controller clock
  */
static void DMA1_Init(void)
{
    /* DMA controller clock enable */
    __HAL_RCC_DMA1_CLK_ENABLE();

    hdma_uart4_rx.Instance = DMA1_Stream2;
    hdma_uart4_rx.Init.Channel = DMA_CHANNEL_4;
    hdma_uart4_rx.Init.Direction = DMA_PERIPH_TO_MEMORY;
    hdma_uart4_rx.Init.PeriphInc = DMA_PINC_DISABLE;
    hdma_uart4_rx.Init.MemInc = DMA_MINC_ENABLE;
    hdma_uart4_rx.Init.PeriphDataAlignment = DMA_MDATAALIGN_BYTE;
    hdma_uart4_rx.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
    hdma_uart4_rx.Init.Mode = DMA_NORMAL;
    hdma_uart4_rx.Init.Priority = DMA_PRIORITY_MEDIUM;
    hdma_uart4_rx.Init.FIFOMode = DMA_FIFOMODE_DISABLE;

    if (HAL_DMA_Init(&hdma_uart4_rx) != HAL_OK)
    {
      Error_Handler();
    }

    __HAL_LINKDMA(&huart4,hdmarx,hdma_uart4_rx);

    /* DMA interrupt init */
    /* DMA1_Stream2_IRQn interrupt configuration */
    HAL_NVIC_SetPriority(DMA1_Stream2_IRQn, 5, 0);
    HAL_NVIC_EnableIRQ(DMA1_Stream2_IRQn);
}
