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

uint16_t ledsMatrix[LEDS_MATRIX_ROW_NUM] = {0};

void leds_matrix_init(void){
  lm_timer_init();
  ARGB_Init();  // Initialization

  leds_matrix_clear();
}

void leds_matrix_clear(void){
	memset(ledsMatrix, 0, LEDS_MATRIX_ROW_NUM *sizeof(uint16_t));

	ARGB_Clear(); // Clear stirp
	while (ARGB_Show() != ARGB_OK); // Update - Option 1
}


void leds_matrix_show_result(void){
	for(uint8_t rowNum = 0; rowNum < LEDS_MATRIX_ROW_NUM; rowNum++){
		for(uint8_t colNum = 0; colNum < LEDS_MATRIX_COL_NUM; colNum++){
			if ( ledsMatrix[rowNum] & (1 << colNum) )
				ARGB_SetRGB(colNum+rowNum*LEDS_MATRIX_COL_NUM, LED_COLOR_OK);
			else
				ARGB_SetRGB(colNum+rowNum*LEDS_MATRIX_COL_NUM, LED_COLOR_FAULT);
		}
	}
	 while (!ARGB_Show());  // Update - Option 2
}
