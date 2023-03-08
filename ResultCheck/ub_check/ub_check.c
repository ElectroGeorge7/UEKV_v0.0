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

//static uint16_t ubError |= UB_SIG_LEV_TIMEOUT;;

static HAL_StatusTypeDef I2C1_Init(void);

HAL_StatusTypeDef ub_check_init(){
	HAL_StatusTypeDef res = 0;
	uint32_t row1SigStart = 0;

	res |= I2C1_Init();
	res |= HAL_I2C_Master_Transmit(&hi2c1, PCF8575_WRITE_ADDR, (uint8_t *)&portInitPacket, sizeof(portExpPacket), UB_CHECK_TIMEOUT);
	res |= HAL_I2C_Master_Receive(&hi2c1, PCF8575_READ_ADDR, (uint8_t *)&portExpPacket, sizeof(portExpPacket), UB_CHECK_TIMEOUT);

	if (!res){
		uartprintf("ub check i2c init");

		uint8_t cnt = 5;
		do{
			if ( ub_check_freq_adjust() == HAL_OK ){
				row1SigStart = HAL_GetTick();

				if ( ub_check_new_res_wait(0, 0, 2*row1SigResRepPeriod, 0xffff) == HAL_OK ){
					row1SigResPeriod = HAL_GetTick() - row1SigStart;
					uartprintf("HAL_GetTick(): %d", HAL_GetTick());
					uartprintf("row1SigResPeriod: %d", row1SigResPeriod);
				}
				return HAL_OK;
			}

		} while (cnt--);

		uartprintf("ub check freq adjust failed");
	} else
		uartprintf("ub check i2c init failed");

	return HAL_ERROR;
}

HAL_StatusTypeDef ub_check_sig_level_wait(uint8_t sigNum , uint8_t sigLev, uint16_t timeout){
	uint16_t data = 0;
	while (timeout--){
		if (HAL_I2C_Master_Receive(&hi2c1, PCF8575_READ_ADDR, (uint8_t *)&data, sizeof(data), UB_CHECK_TIMEOUT) != HAL_OK){
			uartprintf("port expander i2c error");
			return HAL_ERROR;
		}
		data = (sigLev == 0) ? ~data : data;
		if ( data & (1<<sigNum) ) {
			return HAL_OK;
		}
	}

	uartprintf("too long waiting for sig level");
	return HAL_TIMEOUT;
}

HAL_StatusTypeDef ub_check_new_res_wait(uint8_t sigNum, uint8_t sigLev, uint16_t sigMinDur, uint16_t timeout){
	uint32_t row1SigStart = HAL_GetTick();
	uint16_t data = 0;
	while (timeout--){
		if (HAL_I2C_Master_Receive(&hi2c1, PCF8575_READ_ADDR, (uint8_t *)&data, sizeof(data), UB_CHECK_TIMEOUT) != HAL_OK){
			uartprintf("port expander i2c error");
			return HAL_ERROR;
		}
		data = (sigLev == 0) ? ~data : data;
		if ( data & (1<<sigNum) ) {
			if ( ( (HAL_GetTick() - row1SigStart) > sigMinDur ) ){

				if ( ub_check_sig_level_wait(0, (sigLev == 0) ? 1 : 0, 0xffff) == HAL_OK )
					return HAL_OK;
			}
		} else {
			row1SigStart = HAL_GetTick();
		}
	}

	uartprintf("too long waiting for a new result");
	return HAL_TIMEOUT;
}

HAL_StatusTypeDef ub_check_freq_adjust(void){
	uint32_t row1SigStart = 0;
	row1SigResRepPeriod = 0;
	row1SigResPeriod = 0;

	uint32_t i2cPeriodStart = 0;
	uint32_t i2cPeriod = 0;
	uartprintf("HAL_GetTickFreq(): %d", HAL_GetTickFreq());
	i2cPeriodStart = HAL_GetTick();
	HAL_I2C_Master_Receive(&hi2c1, PCF8575_READ_ADDR, (uint8_t *)&portExpPacket, sizeof(portExpPacket), UB_CHECK_TIMEOUT);
	HAL_I2C_Master_Receive(&hi2c1, PCF8575_READ_ADDR, (uint8_t *)&portExpPacket, sizeof(portExpPacket), UB_CHECK_TIMEOUT);
	i2cPeriod = HAL_GetTick() - i2cPeriodStart;
	uartprintf("i2cPeriodStart: %d", i2cPeriodStart);
	uartprintf("i2cPeriod: %d", i2cPeriod);


	if ( ub_check_new_res_wait(0, 0, 2*120, 0xffff) == HAL_OK ){
		row1SigStart = HAL_GetTick();
		if ( ub_check_sig_level_wait(0, 0, 0xffff) == HAL_OK ){
			if ( ub_check_sig_level_wait(0, 1, 0xffff) == HAL_OK ){
				row1SigResRepPeriod = HAL_GetTick() - row1SigStart;
				uartprintf("row1SigStart: %d", row1SigStart);
				uartprintf("row1SigResRepPeriod: %d", row1SigResRepPeriod);
				return HAL_OK;
			}
		}
	}

	return HAL_ERROR;
}


HAL_StatusTypeDef ub_check(){
	uint16_t data = 0;
	uint8_t lastColCirBuf[1] = {0}; uint8_t cirColBufIndex = 0;
	uint8_t lastRowCirBuf[1] = {0}; uint8_t cirRowBufIndex = 0;
	uint8_t newColBitArray = 0;
	uint8_t newRowBitArray = 0;
#define UB_CHECK
#ifdef UB_CHECK
	if ( ub_check_sig_level_wait(0 , 1, 0xffff) == HAL_OK ){
			uint16_t checkRepeatCnt = 500;
			while (checkRepeatCnt--){
				HAL_I2C_Master_Receive(&hi2c1, PCF8575_READ_ADDR, (uint8_t *)&data, sizeof(data), UB_CHECK_TIMEOUT);

				newRowBitArray = (uint8_t)data;
				newColBitArray = (uint8_t) (data >> 8);

				for (uint8_t rowNum = 0; rowNum < UB_MATRIX_ROW_NUM; rowNum++){
					if ( newRowBitArray & lastRowCirBuf[0] & (1<<rowNum) ){
						ubMatrix[rowNum] |= newColBitArray;
						//break;
					}
				}

//				if(++cirColBufIndex == 1){
//					cirColBufIndex = 0;
//				}
//				lastColCirBuf[cirColBufIndex] = newColBitArray;

				if(++cirRowBufIndex == 1){
					cirRowBufIndex = 0;
				}
				lastRowCirBuf[cirRowBufIndex] = newRowBitArray;

			}
			return HAL_OK;
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
static HAL_StatusTypeDef I2C1_Init(void){
  GPIO_InitTypeDef GPIO_InitStruct = {0};

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

  return HAL_OK;
}
