/*
 * rs485_hardware.c
 *
 *  Created on: 5 нояб. 2022 г.
 *      Author: George
 */

#include "rs485_hardware.h"

#define RS485_nRE_Pin GPIO_PIN_2
#define RS485_nRE_GPIO_Port GPIOA
#define RS485_DE_Pin GPIO_PIN_3
#define RS485_DE_GPIO_Port GPIOA

#define TIMEOUT 0xfff

UART_HandleTypeDef huart4;

static void UART4_Init(void);

HAL_StatusTypeDef rs485_receive(uint8_t *rxData, uint16_t size){
	return HAL_UART_Receive(&huart4, rxData, size, TIMEOUT);
}

HAL_StatusTypeDef rs485_transmit(uint8_t *cmd, uint16_t size){
	HAL_StatusTypeDef res = HAL_UART_Transmit(&huart4, cmd, size, TIMEOUT);
	HAL_Delay(10);
	return res;
}

HAL_StatusTypeDef rs485_transmit_w_respond(uint8_t *cmd, uint16_t txSize, uint8_t *respond, uint16_t rxSize){
	HAL_StatusTypeDef res = rs485_transmit(cmd, txSize);

	if (res == HAL_OK){
		HAL_Delay(10);
		return rs485_receive(respond, rxSize);
	}

	return HAL_ERROR;
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

	HAL_GPIO_WritePin(GPIOA, RS485_DE_Pin, GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOA, RS485_nRE_Pin, GPIO_PIN_RESET);


	/*

	HAL_StatusTypeDef rs485Res = HAL_ERROR;

	//rs485Res = HAL_UART_Transmit(&huart4, &rs485String, sizeof(rs485String), 0xfffff);

	char ADR01_cmd[] = ":ADR01;\r";
	char MDL_cmd[] = ":MDL?;\r";
	char REV_cmd[] = ":REV?;\r";
	char RMT0_cmd[] = ":RMT0;\r";
	char RMT1_cmd[] = ":RMT1;\r";
	char RMT2_cmd[] = ":RMT2;\r";
	char OUT1_cmd[] = ":OUT1;\r";
	char OUT0_cmd[] = ":OUT0;\r";
	rs485Res = HAL_UART_Transmit(&huart4, ADR01_cmd, sizeof(ADR01_cmd)-1, 0xfff);
	HAL_Delay(100);
	rs485Res = HAL_UART_Transmit(&huart4, OUT1_cmd, sizeof(OUT1_cmd)-1, 0xfff);
	HAL_Delay(1000);
	rs485Res = HAL_UART_Transmit(&huart4, OUT0_cmd, sizeof(OUT0_cmd)-1, 0xfff);
	HAL_Delay(100);

	rs485Res = HAL_UART_Transmit(&huart4, ADR01_cmd, sizeof(ADR01_cmd)-1, 0xfff);
	HAL_Delay(15);
	rs485Res = HAL_UART_Transmit(&huart4, MDL_cmd, sizeof(MDL_cmd)-1, 0xfff);
	HAL_Delay(15);
	rs485Res = HAL_UART_Receive(&huart4, rs485Buf, sizeof(rs485Buf), 0xfff);
	HAL_Delay(100);

	rs485Res = HAL_UART_Transmit(&huart4, ADR01_cmd, sizeof(ADR01_cmd)-1, 0xfff);
	HAL_Delay(15);
	rs485Res = HAL_UART_Transmit(&huart4, REV_cmd, sizeof(REV_cmd)-1, 0xfff);
	HAL_Delay(15);
	rs485Res = HAL_UART_Receive(&huart4, rs485Buf, sizeof(rs485Buf), 0xfff);
	HAL_Delay(100);

	rs485Res = HAL_UART_Transmit(&huart4, ADR01_cmd, sizeof(ADR01_cmd)-1, 0xfff);
	HAL_Delay(15);
	rs485Res = HAL_UART_Transmit(&huart4, RMT0_cmd, sizeof(RMT0_cmd)-1, 0xfff);
	HAL_Delay(15);
	rs485Res = HAL_UART_Transmit(&huart4, REV_cmd, sizeof(REV_cmd)-1, 0xfff);
	HAL_Delay(15);
	rs485Res = HAL_UART_Receive(&huart4, rs485Buf, sizeof(rs485Buf), 0xfff);
	HAL_Delay(100);

	rs485Res = HAL_UART_Transmit(&huart4, ADR01_cmd, sizeof(ADR01_cmd)-1, 0xfff);
	HAL_Delay(15);
	rs485Res = HAL_UART_Transmit(&huart4, RMT1_cmd, sizeof(RMT0_cmd)-1, 0xfff);
	HAL_Delay(15);
	rs485Res = HAL_UART_Transmit(&huart4, REV_cmd, sizeof(REV_cmd)-1, 0xfff);
	HAL_Delay(15);
	rs485Res = HAL_UART_Receive(&huart4, rs485Buf, sizeof(rs485Buf), 0xfff);
	HAL_Delay(100);

	rs485Res = HAL_UART_Transmit(&huart4, ADR01_cmd, sizeof(ADR01_cmd)-1, 0xfff);
	HAL_Delay(15);
	rs485Res = HAL_UART_Transmit(&huart4, RMT2_cmd, sizeof(RMT0_cmd)-1, 0xfff);
	HAL_Delay(15);
	rs485Res = HAL_UART_Transmit(&huart4, REV_cmd, sizeof(REV_cmd)-1, 0xfff);
	HAL_Delay(15);
	rs485Res = HAL_UART_Receive(&huart4, rs485Buf, sizeof(rs485Buf), 0xfff);
	HAL_Delay(100);

	rs485Res = HAL_UART_Transmit(&huart4, MDL_cmd, sizeof(MDL_cmd)-1, 0xfff);

	rs485Res = HAL_UART_Receive(&huart4, rs485Buf, sizeof(rs485Buf), 0xffff);
	HAL_Delay(100);
*/
	return HAL_OK;
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
