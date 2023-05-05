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

#define ETT_MATRIX_ROW_NUM	RESULT_MATRIX_MAX_ROW_NUM
#define ETT_MATRIX_COL_NUM	RESULT_MATRIX_MAX_COL_NUM
#define ETT_SEG_ROW_NUM	(ETT_MATRIX_ROW_NUM / 2)
#define ETT_SEG_COL_NUM	(ETT_MATRIX_COL_NUM / 2)

/*
 * ett segments sequence:
 *
 * 		1	|	3
 *		---------
 * 		2	|	4
 *
*/

HAL_StatusTypeDef ett_check_init(TestConfig_t conf);
HAL_StatusTypeDef ett_check(uint16_t *resBitMatrix);
void ett_res_clear(void);

#endif /* ETT_CHECK_H_ */
