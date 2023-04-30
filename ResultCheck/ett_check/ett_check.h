/*
 * ett_check.h
 *
 *  Created on: 17 дек. 2022 г.
 *      Author: George
 */

#ifndef ETT_CHECK_H_
#define ETT_CHECK_H_

#include "stm32f4xx_hal.h"

#include "result_check.h"

#define ETT_MATRIX_ROW_NUM	10
#define ETT_MATRIX_COL_NUM	14

/*
 * ett segments sequence:
 *
 * 		1	|	3
 *		---------
 * 		2	|	4
 *
*/

#define ETT_SEG_ROW_NUM	(ETT_MATRIX_ROW_NUM / 2)
#define ETT_SEG_COL_NUM	(ETT_MATRIX_COL_NUM / 2)

extern uint16_t ettMatrix[ETT_MATRIX_ROW_NUM];

#define SPI_ETT_CS1_Pin GPIO_PIN_5
#define SPI_ETT_CS1_GPIO_Port GPIOB
#define SPI_ETT_CS2_Pin GPIO_PIN_12
#define SPI_ETT_CS2_GPIO_Port GPIOB
#define SPI_ETT_CS3_Pin GPIO_PIN_13
#define SPI_ETT_CS3_GPIO_Port GPIOB
#define SPI_ETT_CS4_Pin GPIO_PIN_14
#define SPI_ETT_CS4_GPIO_Port GPIOB

HAL_StatusTypeDef ett_check_init(ResCheckMethod_t method);
HAL_StatusTypeDef ett_check();
HAL_StatusTypeDef ett_res_clear(void);
HAL_StatusTypeDef ett_res_clear(void);

#endif /* ETT_CHECK_H_ */
