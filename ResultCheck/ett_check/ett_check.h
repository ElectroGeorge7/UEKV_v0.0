/*
 * ett_check.h
 *
 *  Created on: 17 дек. 2022 г.
 *      Author: George
 */

#ifndef ETT_CHECK_H_
#define ETT_CHECK_H_

#include "stm32f4xx_hal.h"

#define UB_MATRIX_ROW_NUM	5
#define UB_MATRIX_COL_NUM	7

uint8_t ettMatrix[8];

#define SPI_ETT_CS1_Pin GPIO_PIN_5
#define SPI_ETT_CS1_GPIO_Port GPIOB
#define SPI_ETT_CS2_Pin GPIO_PIN_12
#define SPI_ETT_CS2_GPIO_Port GPIOB
#define SPI_ETT_CS3_Pin GPIO_PIN_13
#define SPI_ETT_CS3_GPIO_Port GPIOB
#define SPI_ETT_CS4_Pin GPIO_PIN_14
#define SPI_ETT_CS4_GPIO_Port GPIOB

HAL_StatusTypeDef ett_check_init();
HAL_StatusTypeDef ett_check();
HAL_StatusTypeDef ett_res_clear(void);

#endif /* ETT_CHECK_H_ */
