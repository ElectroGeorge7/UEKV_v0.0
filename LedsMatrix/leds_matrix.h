/*
 * leds_matrix.h
 *
 *  Created on: 8 окт. 2022 г.
 *      Author: George
 */

#ifndef LEDS_MATRIX_H_
#define LEDS_MATRIX_H_

#include "stm32f4xx_hal.h"

HAL_StatusTypeDef leds_matrix_init(void);
HAL_StatusTypeDef leds_matrix_show_result(void);
HAL_StatusTypeDef leds_matrix_clear(void);

#endif /* LEDS_MATRIX_H_ */
