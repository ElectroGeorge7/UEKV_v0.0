/*
 * lps_task.h
 *
 *  Created on: Mar 12, 2023
 *      Author: George
 */

#ifndef LPS_TASK_H_
#define LPS_TASK_H_

#include "stm32f4xx_hal.h"

typedef struct {
	uint8_t addr;
	char volStr[7];
	char curStr[7];
	char status;
} LpsStatus_t;

void LpsTask(void *argument);

uint8_t lps_get_connected_num(void);
LpsStatus_t *lps_list_get(void);
uint8_t lps_get_update_ready_flag(void);

#endif /* LPS_TASK_H_ */
