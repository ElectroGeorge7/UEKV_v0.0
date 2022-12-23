
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

  for(;;)
  {
	res = osMessageQueueGet(eventQueueHandler, &event, NULL, osWaitForever);

	if( res == osOK )
	{
		if ( event == BUTTON_UP_PRESS_EVENT )
			tempVal++;
		else if ( event == BUTTON_DOWN_PRESS_EVENT )
			tempVal--;
		else if ( event == BUTTON_RIGHT_PRESS_EVENT )
			tempVal = 255;
		else if ( event == BUTTON_LEFT_PRESS_EVENT )
			tempVal = 0;
		else
			tempVal = 127;
	}

	//LCD_Clear(&lcd);
	LCD_Clear();
	snprintf(tempStr, 7, "%d", tempVal);
	LCD_SetCursor( 0, 0 );
	LCD_PrintString(rusString);
	LCD_PrintString(tempStr);
	//LCD_SetCursor(&lcd, 0, 0);
	//LCD_SendString(&lcd, rusString);
	//LCD_SendString(&lcd, tempStr);

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

  //leds_matrix_init();
  //ledsBitMatrix[1] = 0x0c;
  //ledsBitMatrix[7] = 0xc0;
  //leds_matrix_show_result();
  //HAL_Delay(1000);
  //leds_matrix_clear();
  //result_check_init();


  //LCD_Init(&lcd, 0x38, 16, 2);
  //LCD_Backlight(&lcd, 1); //Backlight on
  //LCD_SendString(&lcd, enString);
  //LCD_Clear(&lcd);
  //LCD_SetCursor(&lcd, 0, 1);

  LCD_Init();
  LCD_SetCursor( 0, 0 );
  LCD_PrintString("Hello Odessa");
  LCD_SetCursor( 0, 1 );
  LCD_PrintString("Привет Одесса");

  fatfs_init();
  gfr = f_mount(&sdFatFs, "", 1);
  fRead("Test1.txt", gfileBuf, 10, gbr);
  //LCD_SendString(&lcd, gfileBuf);

  ts_spi_init();

  MX_USB_DEVICE_Init();
  uint8_t usbString[] = "UEKV_v0.0, USB Test\n\r";


  osKernelInitialize();

  eventQueueHandler = osMessageQueueNew (EVENT_QUEUE_OBJECTS, EVENT_QUEUE_OBJ_SIZE, &eventQueue);

  controlTaskHandle = osThreadNew(ControlTask, NULL, &controlTask_attributes);

  osKernelStart();

  while (1)
  {
/*
	  tempRes = Max6675_Read_Temp();
	  snprintf(tsResString, 7, "%f", tempRes);
	  LCD_SetCursor(&lcd, 0, 0);
	  LCD_SendString(&lcd, tsResString);
	  HAL_Delay(1000);
	  LCD_Clear(&lcd);

	  CDC_Transmit_FS(usbString, sizeof(usbString));
	  HAL_Delay(1000);
*/
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
