/*
 * leds_matrix.h
 *
 *  Created on: 8 окт. 2022 г.
 *      Author: George
 */

#ifndef LEDS_MATRIX_H_
#define LEDS_MATRIX_H_

#include "stm32f4xx_hal.h"

#define LEDS_MATRIX_ROW_NUM	10
#define LEDS_MATRIX_COL_NUM	14

HAL_StatusTypeDef leds_matrix_init(void);
HAL_StatusTypeDef leds_matrix_show_result(void);
HAL_StatusTypeDef leds_matrix_clear(void);

#endif /* LEDS_MATRIX_H_ */
