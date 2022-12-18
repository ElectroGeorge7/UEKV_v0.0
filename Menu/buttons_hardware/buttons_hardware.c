
#include "buttons_hardware.h"

#include "main.h"

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin){
	if ( (GPIO_Pin == BUTTON_UP_Pin) || (GPIO_Pin == BUTTON_DOWN_Pin) )
		HAL_GPIO_WritePin(GPIOC, LED_PROCESS_Pin, GPIO_PIN_SET);
	else
		HAL_GPIO_WritePin(GPIOC, LED_PROCESS_Pin, GPIO_PIN_RESET);
}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
HAL_StatusTypeDef buttons_init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();

  /*Configure GPIO pins : BUTTON_UP_Pin BUTTON_DOWN_Pin BUTTON_RIGHT_Pin BUTTON_LEFT_Pin */
  GPIO_InitStruct.Pin = BUTTON_UP_Pin|BUTTON_DOWN_Pin|BUTTON_RIGHT_Pin|BUTTON_LEFT_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /* EXTI interrupt init*/
  HAL_NVIC_SetPriority(EXTI3_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI3_IRQn);

  HAL_NVIC_SetPriority(EXTI4_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI4_IRQn);

  HAL_NVIC_SetPriority(EXTI9_5_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI9_5_IRQn);

  HAL_NVIC_SetPriority(EXTI15_10_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);

  return HAL_OK;
}
