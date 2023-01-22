
#include "buttons_hardware.h"

#include "main.h"
#include "cmsis_os2.h"

extern osMessageQueueId_t eventQueueHandler;

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin){

    osStatus_t osRes;
    Event_t msg;

	if ( GPIO_Pin == BUTTON_UP_Pin ){
		msg.event = BUTTON_UP_PRESS_EVENT;
	} else if ( GPIO_Pin == BUTTON_DOWN_Pin ){
		msg.event = BUTTON_DOWN_PRESS_EVENT;
	} else if ( GPIO_Pin == BUTTON_LEFT_Pin ){
		msg.event = BUTTON_LEFT_PRESS_EVENT;
	} else if ( GPIO_Pin == BUTTON_RIGHT_Pin ){
		msg.event = BUTTON_RIGHT_PRESS_EVENT;
	};

	osRes = osMessageQueuePut (eventQueueHandler, &msg, 0, 0);
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
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /* EXTI interrupt init*/
  /*
   * @note all FreeRTOS safe ISR should have priority <= configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY
   * see FreeRTOSConfig.h
   */
  HAL_NVIC_SetPriority(EXTI3_IRQn, 5, 0);
  HAL_NVIC_EnableIRQ(EXTI3_IRQn);

  HAL_NVIC_SetPriority(EXTI4_IRQn, 5, 0);
  HAL_NVIC_EnableIRQ(EXTI4_IRQn);

  HAL_NVIC_SetPriority(EXTI9_5_IRQn, 5, 0);
  HAL_NVIC_EnableIRQ(EXTI9_5_IRQn);

  HAL_NVIC_SetPriority(EXTI15_10_IRQn, 5, 0);
  HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);

  return HAL_OK;
}
