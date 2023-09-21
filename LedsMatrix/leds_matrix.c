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

  HAL_Delay(10); // find out why on new boards after leds_matrix_clear() it`s need delay
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

HAL_StatusTypeDef leds_matrix_all_green(void){
	for (uint8_t rowNum = 0; rowNum < 10; rowNum++){
		for(uint8_t colNum = 0; (colNum < 14); colNum++){
			ledsMatrix[rowNum][colNum] = RES_CELL_OK;
		}
	}

	leds_matrix_show_result();
	HAL_Delay(1000);
	leds_matrix_clear();
	HAL_Delay(10);
};

HAL_StatusTypeDef leds_matrix_all_red(void){
	for (uint8_t rowNum = 0; rowNum < 10; rowNum++){
		for(uint8_t colNum = 0; (colNum < 14); colNum++){
			ledsMatrix[rowNum][colNum] = RES_CELL_FAULT;
		}
	}

	leds_matrix_show_result();
	HAL_Delay(1000);
	leds_matrix_clear();
	HAL_Delay(10);
};


typedef enum {
	no = 0,
	red,
	orange,
	yellow,
	green,
	blue,
	indigo,
	violet
} LedsColor_t;

static void leds_matrix_color_set(LedsColor_t image[LEDS_MATRIX_ROW_NUM][LEDS_MATRIX_COL_NUM]){
	uint8_t curRowNum = 0;
	uint8_t curColNum = 0;

	for(uint8_t rowNum = 0; rowNum < LEDS_MATRIX_ROW_NUM; rowNum++){

		curRowNum = LEDS_MATRIX_ROW_NUM - 1 - rowNum;

		for(uint8_t colNum = 0; colNum < LEDS_MATRIX_COL_NUM; colNum++){

			curColNum = ( rowNum % 2 ) ? colNum : LEDS_MATRIX_COL_NUM - 1 - colNum;

			if ( image[rowNum][colNum] == red )
				ARGB_SetRGB(curColNum+curRowNum*LEDS_MATRIX_COL_NUM, 0x30, 0, 0);
			else if ( image[rowNum][colNum] == orange )
				ARGB_SetRGB(curColNum+curRowNum*LEDS_MATRIX_COL_NUM, 0x20, 0x10, 0);
			else if ( image[rowNum][colNum] == yellow )
				ARGB_SetRGB(curColNum+curRowNum*LEDS_MATRIX_COL_NUM, 0x20, 0x20, 0);
			else if ( image[rowNum][colNum] == green )
				ARGB_SetRGB(curColNum+curRowNum*LEDS_MATRIX_COL_NUM, 0, 0x30, 0);
			else if ( image[rowNum][colNum] == blue )
				ARGB_SetRGB(curColNum+curRowNum*LEDS_MATRIX_COL_NUM, 0, 0x20, 0x20);
			else if ( image[rowNum][colNum] == indigo )
				ARGB_SetRGB(curColNum+curRowNum*LEDS_MATRIX_COL_NUM, 0, 0, 0x30);
			else if ( image[rowNum][colNum] == violet )
				ARGB_SetRGB(curColNum+curRowNum*LEDS_MATRIX_COL_NUM, 0x20, 0, 0x20);
			else if ( image[rowNum][colNum] == no )
				ARGB_SetRGB(curColNum+curRowNum*LEDS_MATRIX_COL_NUM, LED_COLOR_NO);
		}
	}

	while (ARGB_Show() != ARGB_OK);  // Update - Option 2

	HAL_Delay(250);
}

HAL_StatusTypeDef leds_matrix_milandr(void){
	LedsColor_t image[LEDS_MATRIX_ROW_NUM][LEDS_MATRIX_COL_NUM] = {
			{no,	no,	no,	orange,	orange,	no,	no,	no,	no,	orange,	orange,	no,	no,	no},
			{no,	no,	no,	orange,	orange,	no,	no,	no,	no,	orange,	orange,	no,	no,	no},
			{no,	orange,	orange,	no,	orange,	orange,	no,	no,	orange,	orange,	no,	orange,	orange,	no},
			{no,	orange,	orange,	no,	orange,	orange,	no,	no,	orange,	orange,	no,	orange,	orange,	no},
			{no,	orange,	orange,	no,	no,	orange,	orange,	orange,	orange,	no,	no,	orange,	orange,	no},
			{no,	orange,	orange,	no,	no,	orange,	orange,	orange,	orange,	no,	no,	orange,	orange,	no},
			{orange,	orange,	no,	no,	no,	no,	orange,	orange,	no,	no,	no,	no,	orange,	orange},
			{orange,	orange,	no,	no,	no,	no,	orange,	orange,	no,	no,	no,	no,	orange,	orange},
			{orange,	orange,	no,	no,	no,	no,	no,	no,	no,	no,	no,	no,	orange,	orange},
			{orange,	orange,	no,	no,	no,	no,	no,	no,	no,	no,	no,	no,	orange,	orange}
	};

	leds_matrix_color_set(image);

	HAL_Delay(750);

	ARGB_Clear(); // Clear strip
	while (ARGB_Show() != ARGB_OK); // Update - Option 1

	HAL_Delay(10);
}

HAL_StatusTypeDef leds_matrix_rainbow(void){
	LedsColor_t image[LEDS_MATRIX_ROW_NUM][LEDS_MATRIX_COL_NUM] = {
			{no,	no,	no,	no,	no,	no,	no,	no,	no,	no,	no,	no,	no,	no},
			{no,	no,	no,	no,	no,	no,	no,	no,	no,	no,	no,	no,	no,	no},
			{no,	no,	no,	no,	no,	no,	no,	no,	no,	no,	no,	no,	no,	no},
			{violet,	no,	no,	no,	no,	no,	no,	no,	no,	no,	no,	no,	no,	no},
			{indigo,	no,	no,	no,	no,	no,	no,	no,	no,	no,	no,	no,	no,	no},
			{blue,	no,	no,	no,	no,	no,	no,	no,	no,	no,	no,	no,	no,	no},
			{green,	no,	no,	no,	no,	no,	no,	no,	no,	no,	no,	no,	no,	no},
			{yellow,	no,	no,	no,	no,	no,	no,	no,	no,	no,	no,	no,	no,	no},
			{orange,	no,	no,	no,	no,	no,	no,	no,	no,	no,	no,	no,	no,	no},
			{red,	no,	no,	no,	no,	no,	no,	no,	no,	no,	no,	no,	no,	no}
	};

	leds_matrix_color_set(image);

	image[0][1] = no;
	image[1][1] = no;
	image[2][1] = violet;
	image[3][1] = indigo;
	image[4][1] = blue;
	image[5][1] = green;
	image[6][1] = yellow;
	image[7][1] = orange;
	image[8][1] = red;
	image[9][1] = no;

	leds_matrix_color_set(image);

	image[0][2] = no;
	image[1][2] = violet;
	image[2][2] = indigo;
	image[3][2] = blue;
	image[4][2] = green;
	image[5][2] = yellow;
	image[6][2] = orange;
	image[7][2] = red;
	image[8][2] = no;
	image[9][2] = no;

	leds_matrix_color_set(image);

	image[0][3] = violet;
	image[1][3] = indigo;
	image[2][3] = blue;
	image[3][3] = green;
	image[4][3] = yellow;
	image[5][3] = orange;
	image[6][3] = red;
	image[7][3] = no;
	image[8][3] = no;
	image[9][3] = no;

	leds_matrix_color_set(image);

	image[0][4] = no;
	image[1][4] = violet;
	image[2][4] = indigo;
	image[3][4] = blue;
	image[4][4] = green;
	image[5][4] = yellow;
	image[6][4] = orange;
	image[7][4] = red;
	image[8][4] = no;
	image[9][4] = no;

	leds_matrix_color_set(image);

	image[0][5] = no;
	image[1][5] = no;
	image[2][5] = violet;
	image[3][5] = indigo;
	image[4][5] = blue;
	image[5][5] = green;
	image[6][5] = yellow;
	image[7][5] = orange;
	image[8][5] = red;
	image[9][5] = no;

	leds_matrix_color_set(image);

	image[0][6] = no;
	image[1][6] = no;
	image[2][6] = no;
	image[3][6] = violet;
	image[4][6] = indigo;
	image[5][6] = blue;
	image[6][6] = green;
	image[7][6] = yellow;
	image[8][6] = orange;
	image[9][6] = red;

	leds_matrix_color_set(image);

	image[0][7] = no;
	image[1][7] = no;
	image[2][7] = no;
	image[3][7] = violet;
	image[4][7] = indigo;
	image[5][7] = blue;
	image[6][7] = green;
	image[7][7] = yellow;
	image[8][7] = orange;
	image[9][7] = red;

	leds_matrix_color_set(image);

	image[0][8] = no;
	image[1][8] = no;
	image[2][8] = violet;
	image[3][8] = indigo;
	image[4][8] = blue;
	image[5][8] = green;
	image[6][8] = yellow;
	image[7][8] = orange;
	image[8][8] = red;
	image[9][8] = no;

	leds_matrix_color_set(image);

	image[0][9] = no;
	image[1][9] = violet;
	image[2][9] = indigo;
	image[3][9] = blue;
	image[4][9] = green;
	image[5][9] = yellow;
	image[6][9] = orange;
	image[7][9] = red;
	image[8][9] = no;
	image[9][9] = no;

	leds_matrix_color_set(image);

	image[0][10] = violet;
	image[1][10] = indigo;
	image[2][10] = blue;
	image[3][10] = green;
	image[4][10] = yellow;
	image[5][10] = orange;
	image[6][10] = red;
	image[7][10] = no;
	image[8][10] = no;
	image[9][10] = no;

	leds_matrix_color_set(image);

	image[0][11] = no;
	image[1][11] = violet;
	image[2][11] = indigo;
	image[3][11] = blue;
	image[4][11] = green;
	image[5][11] = yellow;
	image[6][11] = orange;
	image[7][11] = red;
	image[8][11] = no;
	image[9][11] = no;

	leds_matrix_color_set(image);

	image[0][12] = no;
	image[1][12] = no;
	image[2][12] = violet;
	image[3][12] = indigo;
	image[4][12] = blue;
	image[5][12] = green;
	image[6][12] = yellow;
	image[7][12] = orange;
	image[8][12] = red;
	image[9][12] = no;

	leds_matrix_color_set(image);

	image[0][13] = no;
	image[1][13] = no;
	image[2][13] = no;
	image[3][13] = violet;
	image[4][13] = indigo;
	image[5][13] = blue;
	image[6][13] = green;
	image[7][13] = yellow;
	image[8][13] = orange;
	image[9][13] = red;

	leds_matrix_color_set(image);

	ARGB_Clear(); // Clear strip
	while (ARGB_Show() != ARGB_OK); // Update - Option 1

	HAL_Delay(10);

};


