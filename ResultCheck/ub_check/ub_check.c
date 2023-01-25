/*
 * pcf8575.c
 *
 *  Created on: Oct 1, 2022
 *      Author: George
 */

#include "ub_check.h"
#include <string.h>

#include "cmsis_os2.h"

#include "terminal.h"

#define PCF8575_READ_ADDR	0x41
#define PCF8575_WRITE_ADDR	0x40

#define UB_CHECK_TIMEOUT	0xffff
#define UB_CHECK_REPEAT_CNT	16

I2C_HandleTypeDef hi2c1;

uint16_t portExpPacket = 0;
uint16_t portInitPacket = 0xffff;
uint8_t rowBitArray = 0;
uint8_t colBitArray = 0;

uint32_t row1SigStart = 0;				// HAL tick when signal has started on row1
uint32_t row1SigResRepPeriod = 0;		// repetition period of one result based on row1 signal
uint32_t row1SigResPeriod = 0;			// period of new results based on row1 signal

uint8_t ubMatrix[8] = {0};

static void I2C1_Init(void);


HAL_StatusTypeDef ub_check_init(){
	I2C1_Init();
	HAL_I2C_Master_Transmit(&hi2c1, PCF8575_WRITE_ADDR, (uint8_t *)&portInitPacket, sizeof(portExpPacket), UB_CHECK_TIMEOUT);
	HAL_I2C_Master_Receive(&hi2c1, PCF8575_READ_ADDR, (uint8_t *)&portExpPacket, sizeof(portExpPacket), UB_CHECK_TIMEOUT);

	uartprintf("UB check I2C init");

	uint16_t checkRepeatCnt = 0xff;
	uint32_t row1SigResStart = 0;
	while (checkRepeatCnt--){
		//ub_check_temp();
		ub_check_freq_adjust();

		row1SigResStart = HAL_GetTick();
		checkRepeatCnt = 0xffff;
		while (checkRepeatCnt--){
			HAL_I2C_Master_Receive(&hi2c1, PCF8575_READ_ADDR, (uint8_t *)&portExpPacket, sizeof(portExpPacket), UB_CHECK_TIMEOUT);
			if (~portExpPacket & 1) {
				//uartprintf("HAL_GetTick() - row1SigResStart: %d", HAL_GetTick() - row1SigResStart);
				if ( ( (HAL_GetTick() - row1SigResStart) > (2*row1SigResRepPeriod) ) ){
					uartprintf("HAL_GetTick(): %d", HAL_GetTick());
					uartprintf("Result repeat end ");

					checkRepeatCnt = 0xffff;
					while (checkRepeatCnt--){
						HAL_I2C_Master_Receive(&hi2c1, PCF8575_READ_ADDR, (uint8_t *)&portExpPacket, sizeof(portExpPacket), UB_CHECK_TIMEOUT);
						if ( portExpPacket & 1 ){
							row1SigResPeriod = HAL_GetTick() - row1SigStart;

							uartprintf("HAL_GetTick(): %d", HAL_GetTick());
							uartprintf("row1SigResPeriod: %d", row1SigResPeriod);
							return HAL_OK;
						}
					}
				}
			} else {
				row1SigResStart = HAL_GetTick();
				//HAL_Delay(1);
			}
		}
	}

	return HAL_ERROR;
}

HAL_StatusTypeDef ub_check_freq_adjust(){
	uint16_t checkRepeatCnt = 0xff;
	uint8_t rowBitArray = 0;

	uint32_t i2cPeriodStart = 0;
	uint32_t i2cPeriod = 0;

	uartprintf("HAL_GetTickFreq(): %d", HAL_GetTickFreq());
	i2cPeriodStart = HAL_GetTick();
	HAL_I2C_Master_Receive(&hi2c1, PCF8575_READ_ADDR, (uint8_t *)&portExpPacket, sizeof(portExpPacket), UB_CHECK_TIMEOUT);
	HAL_I2C_Master_Receive(&hi2c1, PCF8575_READ_ADDR, (uint8_t *)&portExpPacket, sizeof(portExpPacket), UB_CHECK_TIMEOUT);
	i2cPeriod = HAL_GetTick() - i2cPeriodStart;
	uartprintf("i2cPeriodStart: %d", i2cPeriodStart);
	uartprintf("i2cPeriod: %d", i2cPeriod);

	row1SigStart = 0;
	row1SigResRepPeriod = 0;
	row1SigResPeriod = 0;

	uint32_t row1SigResStart = HAL_GetTick();
	checkRepeatCnt = 0xffff;
	while (checkRepeatCnt--){
		HAL_I2C_Master_Receive(&hi2c1, PCF8575_READ_ADDR, (uint8_t *)&portExpPacket, sizeof(portExpPacket), UB_CHECK_TIMEOUT);
		if (~portExpPacket & 1) {
			if ( ( (HAL_GetTick() - row1SigResStart) > (2*120) ) ){
				break;
			}
		} else {
			row1SigResStart = HAL_GetTick();
		}
	}

	while (checkRepeatCnt--){
		HAL_I2C_Master_Receive(&hi2c1, PCF8575_READ_ADDR, (uint8_t *)&portExpPacket, sizeof(portExpPacket), UB_CHECK_TIMEOUT);
		if ( portExpPacket & 1 ){
			row1SigStart = HAL_GetTick();

			while (checkRepeatCnt--){
				HAL_I2C_Master_Receive(&hi2c1, PCF8575_READ_ADDR, (uint8_t *)&portExpPacket, sizeof(portExpPacket), UB_CHECK_TIMEOUT);
				if ( (~portExpPacket) & 1 ){

					while (checkRepeatCnt--){
						HAL_I2C_Master_Receive(&hi2c1, PCF8575_READ_ADDR, (uint8_t *)&portExpPacket, sizeof(portExpPacket), UB_CHECK_TIMEOUT);
						if ( portExpPacket & 1 ){
							row1SigResRepPeriod = HAL_GetTick() - row1SigStart;

							uartprintf("row1SigStart: %d", row1SigStart);
							uartprintf("row1SigResRepPeriod: %d", row1SigResRepPeriod);
							return HAL_OK;
						}
					}
				}
			}
		}
	}

	return HAL_ERROR;
}

HAL_StatusTypeDef ub_row1_sig_wait(){
	uint16_t checkRepeatCnt = 0xffff;

	uint32_t row1SigResStart = HAL_GetTick();
	while (checkRepeatCnt--){
		HAL_I2C_Master_Receive(&hi2c1, PCF8575_READ_ADDR, (uint8_t *)&portExpPacket, sizeof(portExpPacket), UB_CHECK_TIMEOUT);
		if (~portExpPacket & 1) {
			if ( ( (HAL_GetTick() - row1SigResStart) > (2*120) ) ){
				break;
			}
		} else {
			row1SigResStart = HAL_GetTick();
		}
	}

	while (checkRepeatCnt--){
		HAL_I2C_Master_Receive(&hi2c1, PCF8575_READ_ADDR, (uint8_t *)&portExpPacket, sizeof(portExpPacket), UB_CHECK_TIMEOUT);
		if ( portExpPacket & 1 )
			return HAL_OK;
	}

	return HAL_ERROR;
}

HAL_StatusTypeDef ub_check(){
	uint16_t checkRepeatCnt = 0xff;
	uint8_t checkedRowFlags = 0;
#define UB_CHECK
#ifdef UB_CHECK

	while (checkRepeatCnt--){
		HAL_I2C_Master_Receive(&hi2c1, PCF8575_READ_ADDR, (uint8_t *)&portExpPacket, sizeof(portExpPacket), UB_CHECK_TIMEOUT);
		if ( portExpPacket & 1 ){

			checkRepeatCnt = 500;
			while (checkRepeatCnt--){
				HAL_I2C_Master_Receive(&hi2c1, PCF8575_READ_ADDR, (uint8_t *)&portExpPacket, sizeof(portExpPacket), UB_CHECK_TIMEOUT);

				rowBitArray = (uint8_t)portExpPacket;
				colBitArray = (uint8_t) (portExpPacket >> 8);

				for (uint8_t rowNum = 0; rowNum < UB_MATRIX_ROW_NUM; rowNum++){
					if ( rowBitArray & (1<<rowNum) ){
						ubMatrix[rowNum] |= colBitArray;
						//break;
					}
				}
			}

			return HAL_OK;

		}
	}
#else
	ubMatrix[0] = 0xff;
	ubMatrix[1] = 0x7f;
	ubMatrix[2] = 0x3f;
	ubMatrix[3] = 0x1f;
	ubMatrix[4] = 0x0f;
	ubMatrix[5] = 0x07;
	ubMatrix[6] = 0x03;
	ubMatrix[7] = 0x01;
#endif
  return HAL_OK;
}

HAL_StatusTypeDef ub_res_clear(void){
	memset(ubMatrix, 0, sizeof(ubMatrix));

	portExpPacket = 0;
	rowBitArray = 0;
	colBitArray = 0;

	return HAL_OK;
}

/**
  * @brief I2C1 Initialization Function
  * @param None
  * @retval None
  */
static void I2C1_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  __HAL_RCC_I2C1_CLK_ENABLE();

  hi2c1.Instance = I2C1;
  hi2c1.Init.ClockSpeed = 100000;
  hi2c1.Init.DutyCycle = I2C_DUTYCYCLE_2;
  hi2c1.Init.OwnAddress1 = 0;
  hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c1.Init.OwnAddress2 = 0;
  hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c1) != HAL_OK)
  {
    Error_Handler();
  }

  __HAL_RCC_GPIOB_CLK_ENABLE();
  /**I2C1 GPIO Configuration
  PB6     ------> I2C1_SCL
  PB7     ------> I2C1_SDA
  */
  GPIO_InitStruct.Pin = GPIO_PIN_6|GPIO_PIN_7;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_OD;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  GPIO_InitStruct.Alternate = GPIO_AF4_I2C1;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
}
