#include "rch_timer.h"

#include "main.h"

#include "terminal.h"

#include <string.h>
#include "result_check.h"
#include "ub_check.h"
#include "leds_matrix.h"

extern uint16_t ledsBitMatrix[];

TIM_HandleTypeDef htim9;

static void TIM9_Init(void);

///@todo remove long functions from interrupt callback (use RTOS)
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	uint8_t rowNum = UB_MATRIX_ROW_NUM;

	if (htim->Instance == TIM13) {

		HAL_IncTick();

	} else if (htim->Instance == TIM9) {

		uartprintf("TIM9 interrupt time: %d", HAL_GetTick());



		ub_check();
		for (uint8_t rowNum = 0; rowNum < UB_MATRIX_ROW_NUM; rowNum++){
			resultMatrix[rowNum] |= ubMatrix[rowNum];
		}
		ub_res_clear();

		leds_matrix_clear();
		HAL_Delay(50);
		memcpy(ledsBitMatrix, resultMatrix, sizeof(resultMatrix));
		//ledsBitMatrix[1] = 0xc0;
		//ledsBitMatrix[7] = 0x0c;
		leds_matrix_show_result();
		result_check_clear();

		HAL_GPIO_TogglePin(GPIOC, LED_PROCESS_Pin);

	 }
}

HAL_StatusTypeDef rch_timer_init(void){
	TIM9_Init();
	// phase alignment on the first signal of row1
	uartprintf("phase alignment on the first signal of row1: wait");
	while (ub_row1_sig_wait() != HAL_OK){};
	HAL_TIM_Base_Start_IT(&htim9);
	uartprintf("phase alignment on the first signal of row1: ok");

    return HAL_OK;
}

extern uint32_t row1SigResPeriod;
extern uint32_t row1SigResRepPeriod;
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
  // TIM9 clock value is 180MHz, because of x2 of APB2 clock for timers (from RefMan)
  htim9.Instance = TIM9;
  htim9.Init.Prescaler = 44999;
  htim9.Init.CounterMode = TIM_COUNTERMODE_UP;
  // interrupts timing adjustment to result updating freq
  htim9.Init.Period = (uint16_t) ((( 180000000 / ( htim9.Init.Prescaler + 1 ) ) * (row1SigResPeriod /*- 2*row1SigResRepPeriod*/)) / 1000);
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
  HAL_NVIC_SetPriority(TIM1_BRK_TIM9_IRQn, 7, 0);
  HAL_NVIC_EnableIRQ(TIM1_BRK_TIM9_IRQn);

  //HAL_TIM_Base_Start_IT(&htim9);

}
