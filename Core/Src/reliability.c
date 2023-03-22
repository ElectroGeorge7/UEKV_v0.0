/*
 * reliability.c
 *
 *  Created on: 22 мар. 2023 г.
 *      Author: George
 */

#include "reliability.h"

extern RTC_HandleTypeDef hrtc;

#define BACKUP_COUNT 20
/* Backup registers table */
uint32_t aBKPDataReg[BACKUP_COUNT] =
{
  RTC_BKP_DR0,  RTC_BKP_DR1,  RTC_BKP_DR2,
  RTC_BKP_DR3,  RTC_BKP_DR4,  RTC_BKP_DR5,
  RTC_BKP_DR6,  RTC_BKP_DR7,  RTC_BKP_DR8,
  RTC_BKP_DR9,  RTC_BKP_DR10, RTC_BKP_DR11,
  RTC_BKP_DR12, RTC_BKP_DR13, RTC_BKP_DR14,
  RTC_BKP_DR15, RTC_BKP_DR16, RTC_BKP_DR17,
  RTC_BKP_DR18, RTC_BKP_DR19
};


void bkp_write_data(uint8_t bckReg, uint32_t data){
	HAL_RTCEx_BKUPWrite(&hrtc, aBKPDataReg[bckReg], data);
}


uint32_t bkp_read_data(uint8_t bckReg){
    return HAL_RTCEx_BKUPRead(&hrtc, aBKPDataReg[bckReg]);
}

