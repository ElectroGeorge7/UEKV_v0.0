
/* Includes ------------------------------------------------------------------*/
#include "main.h"

#include "stm32f4xx_sysclock.h"

#include "terminal.h"

#include "buttons_hardware.h"
#include "rtc_hardware.h"
#include "reliability.h"

#include "leds_matrix.h"
#include "result_check.h"
#include "ts_spi.h"
#include "MAX6675.h"

//#include "LCD_i2c.h"
#include "LCD1602.h"
#include <stdio.h>

#include "menu.h"

#include "usb_device.h"

#include "cmsis_os2.h"
#include "control_task.h"
#include "storage_task.h"
#include "ub_check_task.h"
#include "lps_task.h"

TIM_HandleTypeDef htim12;

/* Private function prototypes -----------------------------------------------*/

static void MX_GPIO_Init(void);
static void MX_TIM12_Init(void);

extern uint16_t ledsBitMatrix[];

osThreadId_t controlTaskHandle;
const osThreadAttr_t controlTask_attributes = {
  .name = "controlTask",
  .priority = (osPriority_t) osPriorityNormal5,
  .stack_size = 1200 * 4
};

osThreadId_t storageTaskHandle;
const osThreadAttr_t storageTask_attributes = {
  .name = "storageTask",
  .priority = (osPriority_t) osPriorityNormal3,
  .stack_size = 1300 * 4
};

osThreadId_t ubCheckTaskHandle;
const osThreadAttr_t ubCheckTask_attributes = {
  .name = "ubCheckTask",
  .priority = (osPriority_t) osPriorityNormal3,
  .stack_size = 500 * 4
};

osThreadId_t lpsTaskHandle;
const osThreadAttr_t lpsTask_attributes = {
  .name = "lpsTask",
  .priority = (osPriority_t) osPriorityNormal2,
  .stack_size = 350 * 4
};

#define EVENT_QUEUE_OBJECTS 3
#define EVENT_QUEUE_OBJ_SIZE sizeof(Event_t)
osMessageQueueId_t eventQueueHandler;
const osMessageQueueAttr_t eventQueue = {
	.name = "eventQueue"
};

#define LOG_QUEUE_OBJECTS 3
#define LOG_QUEUE_OBJ_SIZE sizeof(Log_t)
osMessageQueueId_t logQueueHandler;
const osMessageQueueAttr_t logQueue = {
	.name = "logQueue"
};

osSemaphoreId_t ubCheckSem;
osSemaphoreId_t lpsRespondSem;
osEventFlagsId_t testEvents;

int main(void)
{
  /* Configure the system clock */
  SystemClock_Config();

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();
  HAL_Delay(500);

  terminal_init();
  uartprintf("UEKV v.1");
  uartprintf("UART terminal init");

  MX_USB_DEVICE_Init();
  HAL_Delay(500);
  usbprintf("USB terminal init");

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  buttons_init();

  LCD_Init();

  rtc_init();

  ts_spi_init();

  if ( (bkp_read_data(UEKV_LAST_STATE_REG) != UEKV_IDLE_STATE) && (bkp_read_data(UEKV_LAST_STATE_REG) != UEKV_TEST_STATE) ){
	  bkp_write_data(UEKV_LAST_STATE_REG, UEKV_IDLE_STATE);
  }

  ub_check_init();

  osKernelInitialize();

  eventQueueHandler = osMessageQueueNew (EVENT_QUEUE_OBJECTS, EVENT_QUEUE_OBJ_SIZE, &eventQueue);
  logQueueHandler = osMessageQueueNew (LOG_QUEUE_OBJECTS, LOG_QUEUE_OBJ_SIZE, &logQueue);

  ubCheckSem = osSemaphoreNew(1U, 0U, NULL);
  if (ubCheckSem == NULL) {
	  uartprintf("Semaphore object not created: ubCheckSem");
  }

  lpsRespondSem = osSemaphoreNew(1U, 0U, NULL);
  if (lpsRespondSem == NULL) {
	  uartprintf("Semaphore object not created: lpsRespondSem");
  }

  testEvents = osEventFlagsNew(NULL);
  if (testEvents == NULL) {
	  uartprintf("Event Flags object not created, handle failure");
  }

  controlTaskHandle = osThreadNew(ControlTask, NULL, &controlTask_attributes);
  storageTaskHandle = osThreadNew(StorageTask, NULL, &storageTask_attributes);
  lpsTaskHandle = osThreadNew(LpsTask, NULL, &lpsTask_attributes);
  ubCheckTaskHandle = osThreadNew(UbCheckTask, NULL, &ubCheckTask_attributes);

  osKernelStart();

  while (1)
  {

  }
 
}


/**
  * @brief TIM12 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM12_Init(void)
{

  /* USER CODE BEGIN TIM12_Init 0 */

  /* USER CODE END TIM12_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_OC_InitTypeDef sConfigOC = {0};

  /* USER CODE BEGIN TIM12_Init 1 */

  /* USER CODE END TIM12_Init 1 */
  htim12.Instance = TIM12;
  htim12.Init.Prescaler = 0;
  htim12.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim12.Init.Period = 65535;
  htim12.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim12.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim12) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim12, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_PWM_Init(&htim12) != HAL_OK)
  {
    Error_Handler();
  }
  sConfigOC.OCMode = TIM_OCMODE_PWM1;
  sConfigOC.Pulse = 0;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
  if (HAL_TIM_PWM_ConfigChannel(&htim12, &sConfigOC, TIM_CHANNEL_1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM12_Init 2 */

  /* USER CODE END TIM12_Init 2 */
  HAL_TIM_MspPostInit(&htim12);

}



/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOC, LED_ERROR_Pin|LED_PROCESS_Pin, GPIO_PIN_SET);

  /*Configure GPIO pins : LED_ERROR_Pin TEMP_SENS_CS1_Pin TEMP_SENS_CS2_Pin LED_PROCESS_Pin */
  GPIO_InitStruct.Pin = LED_ERROR_Pin|LED_PROCESS_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
}



/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
