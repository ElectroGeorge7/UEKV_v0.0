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
#include "result_check.h"

#define LED_RESULT_BRIGHTNESS	30

#define LED_COLOR_OK		0, LED_RESULT_BRIGHTNESS, 0
#define LED_COLOR_FAULT		LED_RESULT_BRIGHTNESS, 0, 0
#define LED_COLOR_NO		0, 0, 0

ResCellStatus_t ledsMatrix[LEDS_MATRIX_ROW_NUM][LEDS_MATRIX_COL_NUM] = {0};

HAL_StatusTypeDef leds_matrix_init(void){
  lm_timer_init();
  ARGB_Init();  // Initialization

  leds_matrix_clear();

  return HAL_OK;
}

HAL_StatusTypeDef leds_matrix_clear(void){
	for (uint8_t rowNum = 0; rowNum < LEDS_MATRIX_ROW_NUM; rowNum++)
		for(uint8_t colNum = 0; colNum < LEDS_MATRIX_COL_NUM; colNum++)
			ledsMatrix[rowNum][colNum] = RES_CELL_NO;

	ARGB_Clear(); // Clear strip
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

			if ( ledsMatrix[rowNum][colNum] == RES_CELL_OK )
				ARGB_SetRGB(curColNum+curRowNum*LEDS_MATRIX_COL_NUM, LED_COLOR_OK);
			else if ( ledsMatrix[rowNum][colNum] == RES_CELL_FAULT )
				ARGB_SetRGB(curColNum+curRowNum*LEDS_MATRIX_COL_NUM, LED_COLOR_FAULT);
			else if ( ledsMatrix[rowNum][colNum] == RES_CELL_NO )
				ARGB_SetRGB(curColNum+curRowNum*LEDS_MATRIX_COL_NUM, LED_COLOR_NO);
		}
	}

	 while (ARGB_Show() != ARGB_OK);  // Update - Option 2
	 return HAL_OK;
}
