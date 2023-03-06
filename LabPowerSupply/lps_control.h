/*
 * lps_control.h
 *
 *  Created on: 5 мар. 2023 г.
 *      Author: George
 */

#ifndef LPS_CONTROL_H_
#define LPS_CONTROL_H_

#include "stm32f4xx_hal.h"

typedef enum LpsOutputState{
	LPS_OUTPUT_OFF,
	LPS_OUTPUT_ON,
} LpsOutputState_t;

HAL_StatusTypeDef lps_find_all(void);
uint8_t lps_get_quantity(void);
HAL_StatusTypeDef lps_ctrl_output(uint8_t addr, LpsOutputState_t state);
HAL_StatusTypeDef lps_read_status(uint8_t addr, uint8_t *rxData, uint16_t size);

#endif /* LPS_CONTROL_H_ */
