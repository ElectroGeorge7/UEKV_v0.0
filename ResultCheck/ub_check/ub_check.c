/*
 * pcf8575.c
 *
 *  Created on: Oct 1, 2022
 *      Author: George
 */

#include "ub_check.h"
#include <string.h>

#include "main.h"
#include "cmsis_os2.h"

#include "terminal.h"

#include "status_leds.h"
#include "rch_timer.h"

#define PCF8575_READ_ADDR	0x41
#define PCF8575_WRITE_ADDR	0x40

#define UB_CHECK_TIMEOUT	0xffff
#define UB_CHECK_REPEAT_CNT	16

I2C_HandleTypeDef hi2c1;
static DMA_HandleTypeDef hdma_tx;
static DMA_HandleTypeDef hdma_rx;

ResCheckMethod_t confUbCheckMeth = AVERAGE_RESULT;

uint8_t ubBitMatrix[UB_MATRIX_ROW_NUM] = {0};

uint16_t portExpPacket = 0;
uint16_t portInitPacket = 0xffff;
uint8_t rowBitArray = 0;
uint8_t colBitArray = 0;

uint32_t row1SigStart = 0;				// HAL tick when signal has started on row1
uint32_t row1SigResRepPeriod = 0;		// repetition period of one result based on row1 signal
uint32_t row1SigResPeriod = 0;			// period of new results based on row1 signal

#define UB_RX_DMA_BUF_SIZE	1000
static uint16_t portExpBuf[UB_RX_DMA_BUF_SIZE] = {0};
static uint16_t colBitsSum[UB_MATRIX_ROW_NUM][UB_MATRIX_COL_NUM] = {0};

static HAL_StatusTypeDef I2C1_Init(void);
static HAL_StatusTypeDef I2C1_DMA_Init(void);

void HAL_I2C_MasterRxCpltCallback(I2C_HandleTypeDef *hi2c){
	uint8_t newColBitArray = 0;
	uint8_t newRowBitArray = 0;

	for (uint32_t i = 0; i < UB_RX_DMA_BUF_SIZE; i++){
		if (portExpBuf[i] != 0){

			newRowBitArray = (uint8_t)portExpBuf[i];
			newColBitArray = (uint8_t)(portExpBuf[i] >> 8);

			for (uint8_t rowNum = 0; rowNum < UB_MATRIX_ROW_NUM; rowNum++){
				if ( newRowBitArray & (1<<rowNum) ){
					for (uint8_t colNum = 0; colNum < UB_MATRIX_COL_NUM; colNum++)
						colBitsSum[rowNum][colNum] += (newColBitArray & (1<<colNum)) ? 1 : 0;
				}
			}
		}
	}

	if (HAL_I2C_GetState(&hi2c1) == HAL_I2C_STATE_READY){
		HAL_I2C_Master_Receive_DMA(&hi2c1, PCF8575_READ_ADDR, (uint8_t *)portExpBuf, sizeof(portExpBuf));
	}

	status_leds_toggle(LED_PROCESS_Pin);
}

HAL_StatusTypeDef ub_check_init(TestConfig_t conf){
	HAL_StatusTypeDef res = HAL_OK;

	confUbCheckMeth = conf.resCheckMethod;

	res |= I2C1_Init();
	res |= HAL_I2C_Master_Receive(&hi2c1, PCF8575_READ_ADDR, (uint8_t *)&portExpPacket, sizeof(portExpPacket), UB_CHECK_TIMEOUT);
	res |= HAL_I2C_Master_Transmit(&hi2c1, PCF8575_WRITE_ADDR, (uint8_t *)&portInitPacket, sizeof(portExpPacket), UB_CHECK_TIMEOUT);
	res |= HAL_I2C_Master_Receive(&hi2c1, PCF8575_READ_ADDR, (uint8_t *)&portExpPacket, sizeof(portExpPacket), UB_CHECK_TIMEOUT);

	if (!res){
		uartprintf("ub check i2c init");

		switch (conf.resCheckMethod){
			case SYNCHRO_RESULT:
				{

#if	DEBUG_UB_SYNCHRO
					uint8_t cnt = 5;
					do{
						if ( ub_check_freq_adjust() == HAL_OK ){
							uint32_t row1SigStart = HAL_GetTick();

							if ( ub_check_new_res_wait(0, 0, 2*row1SigResRepPeriod, 0xffff) == HAL_OK ){
								row1SigResPeriod = HAL_GetTick() - row1SigStart;
								uartprintf("HAL_GetTick(): %d", HAL_GetTick());
								uartprintf("row1SigResPeriod: %d", row1SigResPeriod);
							}

							res |= rch_timer_init(row1SigResPeriod /*- 2*row1SigResRepPeriod*/);
							// phase alignment on the first signal of row1
							uartprintf("phase alignment on the first signal of row1: wait");
							if ( ub_check_sig_level_wait(0, 1, 0xffff) == HAL_OK){
								res |= rch_timer_start();
								uartprintf("phase alignment on the first signal of row1: ok");
								return HAL_OK;
							}

						}

					} while (cnt--);
					uartprintf("ub check freq adjust failed");
#endif

					// инициализация дма
					// активация прерывания
					res |= I2C1_DMA_Init();

					// disable EXTI15_10_IRQn as it was enabled by setting buttons
					__HAL_GPIO_EXTI_CLEAR_IT(GPIO_PIN_15);
					NVIC_ClearPendingIRQ (EXTI15_10_IRQn);
					HAL_NVIC_DisableIRQ(EXTI15_10_IRQn);

					GPIO_InitTypeDef GPIO_InitStruct = {0};

					/* GPIO Ports Clock Enable */
					__HAL_RCC_GPIOB_CLK_ENABLE();

					GPIO_InitStruct.Pin = GPIO_PIN_15;
					GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
					GPIO_InitStruct.Pull = GPIO_PULLUP;
					HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

					/* EXTI interrupt init*/
					/*
					* @note all FreeRTOS safe ISR should have priority <= configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY
					* see FreeRTOSConfig.h
					*/
					HAL_NVIC_SetPriority(EXTI15_10_IRQn, 5, 0);
					HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);

				}
				break;

			case AVERAGE_RESULT:
				res |= I2C1_DMA_Init();
				res |= ub_check_aver_start();
				res |= rch_timer_init(1000 * conf.resCheckPeriod);
				res |= rch_timer_start();
				break;

			default:
				uartprintf("ub check incorrect method");
				break;
		}
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


HAL_StatusTypeDef ub_check_synchro(uint16_t *resBitMatrix){
	uint16_t data = 0;
	uint8_t lastColCirBuf[1] = {0}; uint8_t cirColBufIndex = 0;
	uint8_t lastRowCirBuf[1] = {0}; uint8_t cirRowBufIndex = 0;
	uint8_t newColBitArray = 0;
	uint8_t newRowBitArray = 0;

	if ( ub_check_sig_level_wait(0 , 1, 0xffff) == HAL_OK ){
			uint16_t checkRepeatCnt = 500;
			while (checkRepeatCnt--){
				HAL_I2C_Master_Receive(&hi2c1, PCF8575_READ_ADDR, (uint8_t *)&data, sizeof(data), UB_CHECK_TIMEOUT);

				newRowBitArray = (uint8_t)data;
				newColBitArray = (uint8_t) (data >> 8);

				for (uint8_t rowNum = 0; rowNum < UB_MATRIX_ROW_NUM; rowNum++){
					if ( newRowBitArray & lastRowCirBuf[0] & (1<<rowNum) ){
						ubBitMatrix[rowNum] |= newColBitArray;
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


	if ( resBitMatrix != NULL )
		memcpy(resBitMatrix, ubBitMatrix, sizeof(ubBitMatrix));

  return HAL_OK;
}

HAL_StatusTypeDef ub_check_aver_start(void){
	HAL_StatusTypeDef res = HAL_OK;
	// don`t remove, need to free SDA line if it was blocked
	res = I2C1_Init();

	if ( (res == HAL_OK) && (HAL_I2C_GetState(&hi2c1) == HAL_I2C_STATE_READY) ){
		HAL_I2C_Master_Receive_DMA(&hi2c1, PCF8575_READ_ADDR, (uint8_t *)portExpBuf, sizeof(portExpBuf));
	} else
		return HAL_ERROR;
}

void ub_check_aver_finish(uint16_t *resBitMatrix){
	uint8_t newColBitArray = 0;
	uint8_t newRowBitArray = 0;

//	HAL_DMA_Abort(&hdma_tx);
//	HAL_DMA_Abort_IT(&hdma_tx);
//	HAL_DMA_Abort(&hdma_rx);
//	HAL_DMA_Abort_IT(&hdma_rx);
	HAL_I2C_Master_Abort_IT(&hi2c1, PCF8575_READ_ADDR);

	for (uint8_t rowNum = 0; rowNum < UB_MATRIX_ROW_NUM; rowNum++){
		for (uint8_t colNum = 0; colNum < UB_MATRIX_COL_NUM; colNum++){
			if ( colBitsSum[rowNum][colNum] > 100 )
				ubBitMatrix[rowNum] |= (1<<colNum);
		}
	}

	memset(colBitsSum, 0, sizeof(colBitsSum));

	for (uint8_t rowNum = 0; rowNum < UB_MATRIX_ROW_NUM; rowNum++){
		resultMatrix[rowNum] = ubBitMatrix[rowNum];
	}
}


ResCheckMethod_t ub_check_method_get(void){
	return confUbCheckMeth;
}

void ub_res_clear(void){
	memset(ubBitMatrix, 0, sizeof(ubBitMatrix));

	portExpPacket = 0;
	rowBitArray = 0;
	colBitArray = 0;
}

/**
  * @brief I2C1 Initialization Function
  * @param None
  * @retval None
  */
static HAL_StatusTypeDef I2C1_Init(void){
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  HAL_GPIO_DeInit(GPIOB, GPIO_PIN_6 | GPIO_PIN_7);
  /*
   * After i2c transmission break during one packet reading
   * PCF8575 can set the low level on SDA line, so master packets can`t be sent.
   * We should to toggle SCL line to make SDA free
  */
  GPIO_InitStruct.Pin = GPIO_PIN_6;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  for (uint8_t i = 0; i < 16; i++){
	  HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_6);
	  HAL_Delay(1);
  }

  __HAL_RCC_GPIOB_CLK_ENABLE();
  /* I2C1 GPIO Configuration
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
}

static HAL_StatusTypeDef I2C1_DMA_Init(void){
	HAL_StatusTypeDef res = HAL_OK;

	__HAL_RCC_DMA1_CLK_ENABLE();

	hdma_tx.Instance                 = DMA1_Stream6;

	hdma_tx.Init.Channel             = DMA_CHANNEL_1;
	hdma_tx.Init.Direction           = DMA_MEMORY_TO_PERIPH;
	hdma_tx.Init.PeriphInc           = DMA_PINC_DISABLE;
	hdma_tx.Init.MemInc              = DMA_MINC_ENABLE;
	hdma_tx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
	hdma_tx.Init.MemDataAlignment    = DMA_MDATAALIGN_BYTE;
	hdma_tx.Init.Mode                = DMA_NORMAL;
	hdma_tx.Init.Priority            = DMA_PRIORITY_LOW;
	hdma_tx.Init.FIFOMode            = DMA_FIFOMODE_DISABLE;

	res |= HAL_DMA_Init(&hdma_tx);

	/* Associate the initialized DMA handle to the the I2C handle */
	__HAL_LINKDMA(&hi2c1, hdmatx, hdma_tx);

	/* Configure the DMA handler for Transmission process */
	hdma_rx.Instance                 = DMA1_Stream5;

	hdma_rx.Init.Channel             = DMA_CHANNEL_1;
	hdma_rx.Init.Direction           = DMA_PERIPH_TO_MEMORY;
	hdma_rx.Init.PeriphInc           = DMA_PINC_DISABLE;
	hdma_rx.Init.MemInc              = DMA_MINC_ENABLE;
	hdma_rx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
	hdma_rx.Init.MemDataAlignment    = DMA_MDATAALIGN_BYTE;
	hdma_rx.Init.Mode                = DMA_NORMAL;
	hdma_rx.Init.Priority            = DMA_PRIORITY_HIGH;
	hdma_rx.Init.FIFOMode            = DMA_FIFOMODE_DISABLE;

	res |= HAL_DMA_Init(&hdma_rx);

	/* Associate the initialized DMA handle to the the I2C handle */
	__HAL_LINKDMA(&hi2c1, hdmarx, hdma_rx);

	/* Configure the NVIC for DMA */
	/* NVIC configuration for DMA transfer complete interrupt (I2C1_TX) */
	HAL_NVIC_SetPriority(DMA1_Stream6_IRQn, 1, 0);
	HAL_NVIC_EnableIRQ(DMA1_Stream6_IRQn);

	/* NVIC configuration for DMA transfer complete interrupt (I2C1_RX) */
	HAL_NVIC_SetPriority(DMA1_Stream5_IRQn, 0, 0);
	HAL_NVIC_EnableIRQ(DMA1_Stream5_IRQn);

	/* this IRQs are needed for hal i2c dma funcs*/
	HAL_NVIC_SetPriority(I2C1_ER_IRQn, 0, 1);
	HAL_NVIC_EnableIRQ(I2C1_ER_IRQn);
	HAL_NVIC_SetPriority(I2C1_EV_IRQn, 0, 2);
	HAL_NVIC_EnableIRQ(I2C1_EV_IRQn);

	return HAL_OK;
}
