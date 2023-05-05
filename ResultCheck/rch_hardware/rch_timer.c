#include "rch_timer.h"

#include "main.h"

#include "terminal.h"

#include <string.h>
#include "result_check.h"
#include "ub_check.h"
#include "leds_matrix.h"

#include "cmsis_os2.h"

extern uint16_t ledsBitMatrix[];

TIM_HandleTypeDef htim9;

extern osSemaphoreId_t resCheckSem;

static void TIM9_Init(uint16_t period_ms);

///@todo remove long functions from interrupt callback (use RTOS)
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	uint8_t rowNum = UB_MATRIX_ROW_NUM;

	if (htim->Instance == TIM13) {

		HAL_IncTick();

	} else if (htim->Instance == TIM9) {

		osSemaphoreRelease(resCheckSem);

	 }
}

HAL_StatusTypeDef rch_timer_init(uint16_t period_ms){
	TIM9_Init(period_ms);
    return HAL_OK;
}

HAL_StatusTypeDef rch_timer_start(void){
	HAL_TIM_Base_Start_IT(&htim9);
}

HAL_StatusTypeDef rch_timer_stop(void){
	HAL_TIM_Base_Stop_IT(&htim9);
}


/**
  * @brief TIM9 Initialization Function
  * @param None
  * @retval None
  * @todo Добавить автоматическую подстройку частоты таймера в зависимости от системной частоты
  */
static void TIM9_Init(uint16_t period_ms)
{
  TIM_ClockConfigTypeDef sClockSourceConfig = {0};

  __HAL_RCC_TIM9_CLK_ENABLE();
  // TIM9 clock value is 180MHz, because of x2 of APB2 clock for timers (from RefMan)
  htim9.Instance = TIM9;
  htim9.Init.Prescaler = 44999;
  htim9.Init.CounterMode = TIM_COUNTERMODE_UP;
  // interrupts timing adjustment to result updating freq
  htim9.Init.Period = (uint16_t) ((( 180000000 / ( htim9.Init.Prescaler + 1 ) ) * period_ms) / 1000);
  htim9.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim9.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;
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
  __HAL_TIM_CLEAR_IT(&htim9, TIM_IT_UPDATE);
  NVIC_ClearPendingIRQ (TIM1_BRK_TIM9_IRQn);
  HAL_NVIC_SetPriority(TIM1_BRK_TIM9_IRQn, 7, 0);
  HAL_NVIC_EnableIRQ(TIM1_BRK_TIM9_IRQn);
}
