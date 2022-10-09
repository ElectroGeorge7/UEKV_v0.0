#include "rch_timer.h"

#include "main.h"

TIM_HandleTypeDef htim9;

static void TIM9_Init(void);

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	HAL_GPIO_TogglePin(GPIOC, LED_PROCESS_Pin);
}

HAL_StatusTypeDef rch_timer_init(void){
	TIM9_Init();
    return HAL_OK;
}

/**
  * @brief TIM9 Initialization Function
  * @param None
  * @retval None
  * @todo Добавить автоматическую подстройку частоты таймера в зависимости от системной частоты
  */
static void TIM9_Init(void)
{

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};

  __HAL_RCC_TIM9_CLK_ENABLE();

  htim9.Instance = TIM9;
  htim9.Init.Prescaler = 7199;
  htim9.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim9.Init.Period = 1999;
  htim9.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim9.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim9) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim9, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }

  /* TIM9 interrupt Init */
  HAL_NVIC_SetPriority(TIM1_BRK_TIM9_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(TIM1_BRK_TIM9_IRQn);

  HAL_TIM_Base_Start_IT(&htim9);

}
