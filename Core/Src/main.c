
/* Includes ------------------------------------------------------------------*/
#include "main.h"

#include "stm32f4xx_sysclock.h"

#include "terminal.h"

#include "buttons_hardware.h"
#include "rtc_hardware.h"
#include "reliability.h"
#include "status_leds.h"

#include "result_check.h"
#include "ts_spi.h"
#include "MAX6675.h"

#include "LCD1602.h"
#include <stdio.h>

#include "menu.h"

#include "usb_device.h"

#include "cmsis_os2.h"
#include "control_task.h"
#include "storage_task.h"
#include "lps_task.h"


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

osThreadId_t ettCheckTaskHandle;
const osThreadAttr_t ettCheckTask_attributes = {
  .name = "ettCheckTask",
  .priority = (osPriority_t) osPriorityNormal3,
  .stack_size = 500 * 4
};

osThreadId_t lpsTaskHandle;
const osThreadAttr_t lpsTask_attributes = {
  .name = "lpsTask",
  .priority = (osPriority_t) osPriorityNormal2,
  .stack_size = 500 * 4
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

osSemaphoreId_t resCheckSem;
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
  uartprintf(SOFTWARE_VER);
  uartprintf(HARDWARE_VER);
  uartprintf("UART terminal init");

  MX_USB_DEVICE_Init();
  HAL_Delay(500);
  usbprintf(SOFTWARE_VER);
  usbprintf(HARDWARE_VER);
  usbprintf("USB terminal init");

  status_leds_init();
  buttons_init();

  LCD_Init();

  rtc_init();

  ts_spi_init();

  if ( (bkp_read_data(UEKV_LAST_STATE_REG) != UEKV_IDLE_STATE) && (bkp_read_data(UEKV_LAST_STATE_REG) != UEKV_TEST_STATE) ){
	  bkp_write_data(UEKV_LAST_STATE_REG, UEKV_IDLE_STATE);
  }

  osKernelInitialize();

  eventQueueHandler = osMessageQueueNew (EVENT_QUEUE_OBJECTS, EVENT_QUEUE_OBJ_SIZE, &eventQueue);
  logQueueHandler = osMessageQueueNew (LOG_QUEUE_OBJECTS, LOG_QUEUE_OBJ_SIZE, &logQueue);

  resCheckSem = osSemaphoreNew(1U, 0U, NULL);
  if (resCheckSem == NULL) {
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

  osKernelStart();

  while (1)
  {

  }
 
}


/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
	status_leds_set(LED_ERROR_Pin);
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
