/*
 * reliability.h
 *
 *  Created on: 22 мар. 2023 г.
 *      Author: George
 */

#ifndef RELIABILITY_H_
#define RELIABILITY_H_

#include "stm32f4xx_hal.h"

#define UEKV_LAST_STATE_REG			0
#define UEKV_LAST_TEST_RES_NUM_REG	1

#define UEKV_IDLE_STATE			0x01
#define UEKV_TEST_STATE			0x02

void bkp_write_data(uint8_t bckReg, uint32_t data);
uint32_t bkp_read_data(uint8_t bckReg);

#endif /* RELIABILITY_H_ */
