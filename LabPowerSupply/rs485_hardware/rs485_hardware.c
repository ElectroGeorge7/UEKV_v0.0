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

UART_HandleTypeDef huart4;
uint8_t rs485Buf[32] = {0};

static void UART4_Init(void);

HAL_StatusTypeDef rs485_init(void){
	GPIO_InitTypeDef GPIO_InitStruct = {0};

	__HAL_RCC_GPIOA_CLK_ENABLE();

	/*Configure GPIO pin Output Level */
	HAL_GPIO_WritePin(GPIOA, RS485_nRE_Pin|RS485_DE_Pin, GPIO_PIN_RESET);

	/*Configure GPIO pins : RS485_nRE_Pin RS485_DE_Pin */
	GPIO_InitStruct.Pin = RS485_nRE_Pin|RS485_DE_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

	UART4_Init();

	HAL_GPIO_WritePin(GPIOA, RS485_nRE_Pin|RS485_DE_Pin, GPIO_PIN_SET); // only transmit
	HAL_Delay(100);

	HAL_StatusTypeDef rs485Res = HAL_ERROR;
	char rs485String[] = "Hello RS485 \n";
	//rs485Res = HAL_UART_Transmit(&huart4, &rs485String, sizeof(rs485String), 0xfffff);


	char MDL_cmd[] = ":MDL?;\r\n";
	char ADR01_cmd[] = ":ADR1;\r\n";
	rs485Res = HAL_UART_Transmit(&huart4, ADR01_cmd, sizeof(ADR01_cmd-1), 0xfff);
	HAL_Delay(100);
	rs485Res = HAL_UART_Transmit(&huart4, MDL_cmd, sizeof(MDL_cmd-1), 0xfff);
	HAL_Delay(100);
	rs485Res = HAL_UART_Receive(&huart4, rs485Buf, sizeof(rs485Buf), 0xfff);
	HAL_Delay(100);

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
