
/* Includes ------------------------------------------------------------------*/
#include "main.h"

#include "stm32f4xx_sysclock.h"

#include "buttons_hardware.h"

#include "leds_matrix.h"
#include "result_check.h"
#include "ts_spi.h"
#include "MAX6675.h"

//#include "LCD_i2c.h"
#include "LCD1602.h"
#include <stdio.h>

#include "menu.h"

#include "fatfs.h"

#include "usb_device.h"

#include "cmsis_os2.h"

TIM_HandleTypeDef htim12;

/* Private function prototypes -----------------------------------------------*/

static void MX_GPIO_Init(void);
static void MX_TIM12_Init(void);

extern uint16_t ledsBitMatrix[];

//LCD_I2C_t lcd;

char tsResString[10];
float tempRes=0;

char enString[] = "Hello world";
char rusString[] = "тек пер:";

static FATFS sdFatFs;
static FIL sdFile;
FRESULT gfr;

uint8_t gfileBuf[80] = {0};
uint32_t gbr;
void fRead(char *configFileName, uint8_t *buf, uint32_t num, uint32_t *br){
  FIL readFile;

  do{
      //fr = open_append(&sdFile, "camconf.txt");
      gfr = f_open(&readFile, configFileName, FA_READ);
  }while (gfr);

  gfr = f_read(&readFile, buf, num, br);

  // Close the file
  f_close(&readFile);
}

osThreadId_t controlTaskHandle;
const osThreadAttr_t controlTask_attributes = {
  .name = "controlTask",
  .priority = (osPriority_t) osPriorityNormal3,
  .stack_size = 768 * 4
};

#define EVENT_QUEUE_OBJECTS 10
#define EVENT_QUEUE_OBJ_SIZE 1
osMessageQueueId_t eventQueueHandler;
const osMessageQueueAttr_t eventQueue = {
	.name = "eventQueue"
};

void ControlTask(void *argument)
{
	uint8_t tempVal = 0;
	char tempStr[8] = {0};

	osStatus_t res;
	uint8_t event;

	leds_matrix_init();
	//result_check_init();

  for(;;)
  {
	res = osMessageQueueGet(eventQueueHandler, &event, NULL, osWaitForever);

	if( res == osOK )
	{
		if ( event == BUTTON_UP_PRESS_EVENT )
			menu_view_update(DOWN_CMD);
		else if ( event == BUTTON_DOWN_PRESS_EVENT )
			menu_view_update(DOWN_CMD);
		else if ( event == BUTTON_RIGHT_PRESS_EVENT )
			menu_view_update(SELECT_CMD);
		else if ( event == BUTTON_LEFT_PRESS_EVENT )
			menu_view_update(BACK_CMD);
		else
			menu_view_update(START_CMD);
	}

		// Arrow animation
		ledsBitMatrix[1] = 0xc0;
		leds_matrix_show_result();
		HAL_Delay(200);
		leds_matrix_clear();

		ledsBitMatrix[1] = 0x1e0;
		ledsBitMatrix[2] = 0xc0;
		leds_matrix_show_result();
		HAL_Delay(200);
		leds_matrix_clear();

		ledsBitMatrix[1] = 0x3f0;
		ledsBitMatrix[2] = 0x1e0;
		ledsBitMatrix[3] = 0xc0;
		leds_matrix_show_result();
		HAL_Delay(200);
		leds_matrix_clear();

		ledsBitMatrix[1] = 0x7f8;
		ledsBitMatrix[2] = 0x3f0;
		ledsBitMatrix[3] = 0x1e0;
		ledsBitMatrix[4] = 0xc0;
		leds_matrix_show_result();
		HAL_Delay(200);
		leds_matrix_clear();

		ledsBitMatrix[1] = 0xffc;
		ledsBitMatrix[2] = 0x7f8;
		ledsBitMatrix[3] = 0x3f0;
		ledsBitMatrix[4] = 0x1e0;
		ledsBitMatrix[5] = 0xc0;
		leds_matrix_show_result();
		HAL_Delay(200);
		leds_matrix_clear();

		ledsBitMatrix[1] = 0x1ffe;
		ledsBitMatrix[2] = 0xffc;
		ledsBitMatrix[3] = 0x7f8;
		ledsBitMatrix[4] = 0x3f0;
		ledsBitMatrix[5] = 0x1e0;
		ledsBitMatrix[6] = 0xc0;
		leds_matrix_show_result();
		HAL_Delay(200);
		leds_matrix_clear();

		ledsBitMatrix[1] = 0xc0;
		ledsBitMatrix[2] = 0x1ffe;
		ledsBitMatrix[3] = 0xffc;
		ledsBitMatrix[4] = 0x7f8;
		ledsBitMatrix[5] = 0x3f0;
		ledsBitMatrix[6] = 0x1e0;
		ledsBitMatrix[7] = 0xc0;
		leds_matrix_show_result();
		HAL_Delay(200);
		leds_matrix_clear();

		ledsBitMatrix[1] = 0xc0;
		ledsBitMatrix[2] = 0xc0;
		ledsBitMatrix[3] = 0x1ffe;
		ledsBitMatrix[4] = 0xffc;
		ledsBitMatrix[5] = 0x7f8;
		ledsBitMatrix[6] = 0x3f0;
		ledsBitMatrix[7] = 0x1e0;
		ledsBitMatrix[8] = 0xc0;
		leds_matrix_show_result();
		HAL_Delay(200);
		leds_matrix_clear();

		ledsBitMatrix[1] = 0xc0;
		ledsBitMatrix[2] = 0xc0;
		ledsBitMatrix[3] = 0xc0;
		ledsBitMatrix[4] = 0x1ffe;
		ledsBitMatrix[5] = 0xffc;
		ledsBitMatrix[6] = 0x7f8;
		ledsBitMatrix[7] = 0x3f0;
		ledsBitMatrix[8] = 0x1e0;
		ledsBitMatrix[9] = 0xc0;
		leds_matrix_show_result();
		HAL_Delay(200);
		leds_matrix_clear();

		HAL_Delay(1000);

		ledsBitMatrix[9] = 0xc0;
		leds_matrix_show_result();
		HAL_Delay(200);
		leds_matrix_clear();

		ledsBitMatrix[9] = 0x1e0;
		ledsBitMatrix[8] = 0xc0;
		leds_matrix_show_result();
		HAL_Delay(200);
		leds_matrix_clear();

		ledsBitMatrix[9] = 0x3f0;
		ledsBitMatrix[8] = 0x1e0;
		ledsBitMatrix[7] = 0xc0;
		leds_matrix_show_result();
		HAL_Delay(200);
		leds_matrix_clear();

		ledsBitMatrix[9] = 0x7f8;
		ledsBitMatrix[8] = 0x3f0;
		ledsBitMatrix[7] = 0x1e0;
		ledsBitMatrix[6] = 0xc0;
		leds_matrix_show_result();
		HAL_Delay(200);
		leds_matrix_clear();

		ledsBitMatrix[9] = 0xffc;
		ledsBitMatrix[8] = 0x7f8;
		ledsBitMatrix[7] = 0x3f0;
		ledsBitMatrix[6] = 0x1e0;
		ledsBitMatrix[5] = 0xc0;
		leds_matrix_show_result();
		HAL_Delay(200);
		leds_matrix_clear();

		ledsBitMatrix[9] = 0x1ffe;
		ledsBitMatrix[8] = 0xffc;
		ledsBitMatrix[7] = 0x7f8;
		ledsBitMatrix[6] = 0x3f0;
		ledsBitMatrix[5] = 0x1e0;
		ledsBitMatrix[4] = 0xc0;
		leds_matrix_show_result();
		HAL_Delay(200);
		leds_matrix_clear();

		ledsBitMatrix[9] = 0xc0;
		ledsBitMatrix[8] = 0x1ffe;
		ledsBitMatrix[7] = 0xffc;
		ledsBitMatrix[6] = 0x7f8;
		ledsBitMatrix[5] = 0x3f0;
		ledsBitMatrix[4] = 0x1e0;
		ledsBitMatrix[3] = 0xc0;
		leds_matrix_show_result();
		HAL_Delay(200);
		leds_matrix_clear();

		ledsBitMatrix[9] = 0xc0;
		ledsBitMatrix[8] = 0xc0;
		ledsBitMatrix[7] = 0x1ffe;
		ledsBitMatrix[6] = 0xffc;
		ledsBitMatrix[5] = 0x7f8;
		ledsBitMatrix[4] = 0x3f0;
		ledsBitMatrix[3] = 0x1e0;
		ledsBitMatrix[2] = 0xc0;
		leds_matrix_show_result();
		HAL_Delay(200);
		leds_matrix_clear();

		ledsBitMatrix[9] = 0xc0;
		ledsBitMatrix[8] = 0xc0;
		ledsBitMatrix[7] = 0xc0;
		ledsBitMatrix[6] = 0x1ffe;
		ledsBitMatrix[5] = 0xffc;
		ledsBitMatrix[4] = 0x7f8;
		ledsBitMatrix[3] = 0x3f0;
		ledsBitMatrix[2] = 0x1e0;
		ledsBitMatrix[1] = 0xc0;
		leds_matrix_show_result();
		HAL_Delay(200);
		leds_matrix_clear();

    osDelay(1000);
  }
}


int main(void)
{
  /* Configure the system clock */
  SystemClock_Config();

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();
  HAL_Delay(500);


  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  buttons_init();

  LCD_Init();

  menu_init();

  fatfs_init();
  gfr = f_mount(&sdFatFs, "", 1);

  MX_USB_DEVICE_Init();


  osKernelInitialize();

  eventQueueHandler = osMessageQueueNew (EVENT_QUEUE_OBJECTS, EVENT_QUEUE_OBJ_SIZE, &eventQueue);

  controlTaskHandle = osThreadNew(ControlTask, NULL, &controlTask_attributes);

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
