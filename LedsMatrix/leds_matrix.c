/*
 * leds_matrix.c
 *
 *  Created on: 8 окт. 2022 г.
 *      Author: George
 */


#include "leds_matrix.h"
#include "lm_timer.h"
#include "ARGB.h"
#include <string.h>

#define LEDS_MATRIX_ROW_NUM	10
#define LEDS_MATRIX_COL_NUM	14

#define LED_RESULT_BRIGHTNESS	10

#define LED_COLOR_OK		0, LED_RESULT_BRIGHTNESS, 0
#define LED_COLOR_FAULT		0, 0, 0//LED_RESULT_BRIGHTNESS, 0, 0

uint16_t ledsBitMatrix[LEDS_MATRIX_ROW_NUM] = {0};

HAL_StatusTypeDef leds_matrix_init(void){
  lm_timer_init();
  ARGB_Init();  // Initialization

  leds_matrix_clear();

  return HAL_OK;
}

HAL_StatusTypeDef leds_matrix_clear(void){
	memset(ledsBitMatrix, 0, LEDS_MATRIX_ROW_NUM *sizeof(uint16_t));

	ARGB_Clear(); // Clear stirp
	while (ARGB_Show() != ARGB_OK); // Update - Option 1

	return HAL_OK;
}

///@todo add timeout for ARGB_Show()
HAL_StatusTypeDef leds_matrix_show_result(void){
	uint8_t curRowNum = 0;
	uint8_t curColNum = 0;

	for(uint8_t rowNum = 0; rowNum < LEDS_MATRIX_ROW_NUM; rowNum++){

		curRowNum = LEDS_MATRIX_ROW_NUM - 1 - rowNum;

		for(uint8_t colNum = 0; colNum < LEDS_MATRIX_COL_NUM; colNum++){

			curColNum = ( rowNum % 2 ) ? colNum : LEDS_MATRIX_COL_NUM - 1 - colNum;

			if ( ledsBitMatrix[rowNum] & (1 << colNum) )
				ARGB_SetRGB(curColNum+curRowNum*LEDS_MATRIX_COL_NUM, LED_COLOR_OK);
			else
				ARGB_SetRGB(curColNum+curRowNum*LEDS_MATRIX_COL_NUM, LED_COLOR_FAULT);
		}
	}

	 while (!ARGB_Show());  // Update - Option 2
	 return HAL_OK;
}
