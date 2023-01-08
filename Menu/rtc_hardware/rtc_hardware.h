/*
 * rtc_hardware.h
 *
 *  Created on: Jan 8, 2023
 *      Author: George
 */

#ifndef RTC_HARDWARE_H_
#define RTC_HARDWARE_H_

#include "stm32f4xx_hal.h"

typedef struct {
	uint8_t sec;
	uint8_t min;
	uint8_t hour;
	uint8_t day;
	uint8_t mon;
	uint16_t year;
} DataTime_t;

void rtc_init(void);
HAL_StatusTypeDef rtc_set(DataTime_t *dataTime);
HAL_StatusTypeDef rtc_get(DataTime_t *dataTime);

#endif /* RTC_HARDWARE_H_ */
