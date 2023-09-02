/*
 * status_leds.c
 *
 *  Created on: 2 сент. 2023 г.
 *      Author: George
 */

#include "status_leds.h"


void status_leds_init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOC, LED_ERROR_Pin | LED_PROCESS_Pin, GPIO_PIN_SET);

  /*Configure GPIO pins : LED_ERROR_Pin TEMP_SENS_CS1_Pin TEMP_SENS_CS2_Pin LED_PROCESS_Pin */
  GPIO_InitStruct.Pin = LED_ERROR_Pin|LED_PROCESS_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
}

void status_leds_set(uint16_t leds){
	if (leds & LED_ERROR_Pin)
		HAL_GPIO_WritePin(GPIOC, LED_ERROR_Pin, GPIO_PIN_SET);
	if (leds & LED_PROCESS_Pin)
		HAL_GPIO_WritePin(GPIOC, LED_PROCESS_Pin, GPIO_PIN_SET);
}

void status_leds_reset(uint16_t leds){
	if (leds & LED_ERROR_Pin)
		HAL_GPIO_WritePin(GPIOC, LED_ERROR_Pin, GPIO_PIN_RESET);
	if (leds & LED_PROCESS_Pin)
		HAL_GPIO_WritePin(GPIOC, LED_PROCESS_Pin, GPIO_PIN_RESET);
}

void status_leds_toggle(uint16_t leds){
	if (leds & LED_ERROR_Pin)
		HAL_GPIO_TogglePin(GPIOC, LED_ERROR_Pin);
	if (leds & LED_PROCESS_Pin)
		HAL_GPIO_TogglePin(GPIOC, LED_PROCESS_Pin);
}
