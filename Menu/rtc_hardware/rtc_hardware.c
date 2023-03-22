/*
 * rtc_hardware.c
 *
 *  Created on: Jan 8, 2023
 *      Author: George
 */


#include "rtc_hardware.h"

#include "menu.h"

RTC_HandleTypeDef hrtc;

/**
  * @brief RTC Initialization Function
  * @param None
  * @retval None
  */
void rtc_init(void)
{
	RTC_TimeTypeDef sTime = {0};
	RTC_DateTypeDef sDate = {0};

	hrtc.Instance = RTC;

	// determines if rtc was initialized by calendar year value (0 or not)
	if ( (hrtc.Instance->ISR & RTC_FLAG_INITS) == 0
			){

		/** Initialize RTC Only
		*/
		hrtc.Init.HourFormat = RTC_HOURFORMAT_24;
		hrtc.Init.AsynchPrediv = 127;
		hrtc.Init.SynchPrediv = 255;
		hrtc.Init.OutPut = RTC_OUTPUT_DISABLE;
		hrtc.Init.OutPutPolarity = RTC_OUTPUT_POLARITY_HIGH;
		hrtc.Init.OutPutType = RTC_OUTPUT_TYPE_OPENDRAIN;
		if (HAL_RTC_Init(&hrtc) != HAL_OK)
		{
			Error_Handler();
		}

		/** Initialize RTC and set the Time and Date
		*/
		sTime.Hours = 0x0;
		sTime.Minutes = 0x0;
		sTime.Seconds = 0x0;
		sTime.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
		sTime.StoreOperation = RTC_STOREOPERATION_RESET;
		if (HAL_RTC_SetTime(&hrtc, &sTime, RTC_FORMAT_BCD) != HAL_OK)
		{
			Error_Handler();
		}
		sDate.WeekDay = RTC_WEEKDAY_SUNDAY;
		sDate.Month = RTC_MONTH_JANUARY;
		sDate.Date = 1;
		sDate.Year = 23;

		if (HAL_RTC_SetDate(&hrtc, &sDate, RTC_FORMAT_BIN) != HAL_OK)
		{
			Error_Handler();
		}

	}
}

HAL_StatusTypeDef rtc_set(DataTime_t *dataTime){
	HAL_StatusTypeDef res;
	RTC_TimeTypeDef sTime = {0};
	RTC_DateTypeDef sDate = {0};

	sTime.Seconds = dataTime->sec;
	sTime.Minutes = dataTime->min;
	sTime.Hours = dataTime->hour;

	sDate.Date = dataTime->day;
	sDate.Month = dataTime->mon;
	sDate.Year = dataTime->year - 2000;

	res = HAL_RTC_SetTime(&hrtc, &sTime, RTC_FORMAT_BIN);
	res = HAL_RTC_SetDate(&hrtc, &sDate, RTC_FORMAT_BIN);

	return res;
}

HAL_StatusTypeDef rtc_get(DataTime_t *dataTime){
	HAL_StatusTypeDef res;
	RTC_TimeTypeDef sTime = {0};
	RTC_DateTypeDef sDate = {0};

	res = HAL_RTC_GetTime(&hrtc, &sTime, RTC_FORMAT_BIN);
	res = HAL_RTC_GetDate(&hrtc, &sDate, RTC_FORMAT_BIN);

	dataTime->sec = sTime.Seconds;
	dataTime->min = sTime.Minutes;
	dataTime->hour = sTime.Hours;

	dataTime->day = sDate.Date;
	dataTime->mon = sDate.Month;
	dataTime->year = sDate.Year + 2000;

	return res;
}


/**
* @brief RTC MSP Initialization
* This function configures the hardware resources used in this example
* @param hrtc: RTC handle pointer
* @retval None
*/
void HAL_RTC_MspInit(RTC_HandleTypeDef* hrtc)
{
  if(hrtc->Instance==RTC)
  {
    /* Peripheral clock enable */
    __HAL_RCC_RTC_ENABLE();
  }

}

/**
* @brief RTC MSP De-Initialization
* This function freeze the hardware resources used in this example
* @param hrtc: RTC handle pointer
* @retval None
*/
void HAL_RTC_MspDeInit(RTC_HandleTypeDef* hrtc)
{
  if(hrtc->Instance==RTC)
  {
    /* Peripheral clock disable */
    __HAL_RCC_RTC_DISABLE();
  }

}
