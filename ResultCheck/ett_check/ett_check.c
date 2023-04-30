/*
 * ett_check.c
 *
 *  Created on: 17 дек. 2022 г.
 *      Author: George
 */

#include "ett_check.h"

#include "main.h"
#include "cmsis_os2.h"

#include "terminal.h"

#include "rch_timer.h"

#define ETT_START_FLAG 0x5555
#define ETT_LETSGO_FLAG 0x5005

#define ETT_TIMEOUT 100

SPI_HandleTypeDef hspi1;

uint16_t ettMatrix[ETT_MATRIX_ROW_NUM] = {0};

static void MX_SPI1_Init(void);

static HAL_StatusTypeDef ett_send_flag(uint16_t flag, uint8_t segNum);
static void ett_seg_res_save(uint8_t segNum);


HAL_StatusTypeDef ett_check_init(ResCheckMethod_t method){
	HAL_StatusTypeDef res = HAL_OK;
	MX_SPI1_Init();

	switch (method){
		case EVERY_RESULT:
		case AVERAGE_RESULT_PER_1S:
			res |= rch_timer_init(1000);
			res |= rch_timer_start();
			break;

		case AVERAGE_RESULT_PER_2S:
			res |= rch_timer_init(2000);
			res |= rch_timer_start();
			break;

		case AVERAGE_RESULT_PER_3S:
			res |= rch_timer_init(3000);
			res |= rch_timer_start();
			break;

		case AVERAGE_RESULT_PER_4S:
			res |= rch_timer_init(4000);
			res |= rch_timer_start();
			break;

		case AVERAGE_RESULT_PER_5S:
			res |= rch_timer_init(5000);
			res |= rch_timer_start();
			break;

		case JUST_FAULTES:
			break;

		default:
			res |= rch_timer_init(3000);
			res |= rch_timer_start();
			break;
	}

}

HAL_StatusTypeDef ett_check(){
    if (ett_send_flag(ETT_START_FLAG, 1) == HAL_OK){
      if (ett_send_flag(ETT_LETSGO_FLAG, 1) == HAL_OK){
        ett_seg_res_save(1);
      }
    }

    if (ett_send_flag(ETT_START_FLAG, 2) == HAL_OK){
      if (ett_send_flag(ETT_LETSGO_FLAG, 2) == HAL_OK){
        ett_seg_res_save(2);
      }
    }

    if (ett_send_flag(ETT_START_FLAG, 3) == HAL_OK){
      if (ett_send_flag(ETT_LETSGO_FLAG, 3) == HAL_OK){
        ett_seg_res_save(3);
      }
    }

    if (ett_send_flag(ETT_START_FLAG, 4) == HAL_OK){
      if (ett_send_flag(ETT_LETSGO_FLAG, 4) == HAL_OK){
        ett_seg_res_save(4);
      }
    }

    return HAL_OK;
}

HAL_StatusTypeDef ett_res_clear(void){
	memset(ettMatrix, 0, sizeof(ettMatrix));
	return HAL_OK;
}

uint16_t flagResp = 0;
static HAL_StatusTypeDef ett_send_flag(uint16_t flag, uint8_t segNum){
  uint16_t timeout = ETT_TIMEOUT;
  flagResp = 0;

  HAL_GPIO_WritePin(GPIOB, SPI_ETT_CS1_Pin|SPI_ETT_CS2_Pin|SPI_ETT_CS3_Pin|SPI_ETT_CS4_Pin, GPIO_PIN_SET);

  while (flagResp != flag+1 && timeout--) {

	switch(segNum){
		case 1:
			HAL_GPIO_WritePin(GPIOB, SPI_ETT_CS1_Pin, GPIO_PIN_RESET);
			break;
		case 2:
			HAL_GPIO_WritePin(GPIOB, SPI_ETT_CS2_Pin, GPIO_PIN_RESET);
			break;
		case 3:
			HAL_GPIO_WritePin(GPIOB, SPI_ETT_CS3_Pin, GPIO_PIN_RESET);
			break;
		case 4:
			HAL_GPIO_WritePin(GPIOB, SPI_ETT_CS4_Pin, GPIO_PIN_RESET);
			break;
		default:
			HAL_GPIO_WritePin(GPIOB, SPI_ETT_CS1_Pin|SPI_ETT_CS2_Pin|SPI_ETT_CS3_Pin|SPI_ETT_CS4_Pin, GPIO_PIN_SET);
	};
//	  HAL_SPI_Receive(&hspi1, (uint8_t *)&flagResp, sizeof(flagResp), 0xff);
//	  HAL_SPI_Transmit(&hspi1, &flag, sizeof(flag), 0xff);
	  HAL_SPI_TransmitReceive(&hspi1, (uint8_t *)&flag, (uint8_t *)&flagResp,  1, 0xff);

	  HAL_GPIO_WritePin(GPIOB, SPI_ETT_CS1_Pin|SPI_ETT_CS2_Pin|SPI_ETT_CS3_Pin|SPI_ETT_CS4_Pin, GPIO_PIN_SET);

	  if (flagResp == flag+1) {
		  return HAL_OK;
	  }
  }

  return HAL_ERROR;
}

static void ett_seg_res_save(uint8_t segNum){
	uint16_t segResBuf = 0;
	uint16_t timeout = ETT_TIMEOUT;

	switch(segNum){
		case 1:
			for (uint8_t rowNum = 0; rowNum < (ETT_SEG_ROW_NUM); rowNum++){
				do{
					HAL_GPIO_WritePin(GPIOB, SPI_ETT_CS1_Pin, GPIO_PIN_RESET);
					HAL_SPI_Receive(&hspi1, (uint8_t *)&segResBuf, 1, 0xfff);
					HAL_GPIO_WritePin(GPIOB, SPI_ETT_CS1_Pin, GPIO_PIN_SET);
				} while( ((segResBuf >> 8) != 0) && timeout-- );

				if (timeout == 0)
					return;

				ettMatrix[rowNum] |= segResBuf;
			}
			HAL_GPIO_WritePin(GPIOB, SPI_ETT_CS1_Pin, GPIO_PIN_RESET);
			HAL_SPI_Receive(&hspi1, (uint8_t *)&segResBuf, 1, 0xfff);	// read 6th zero`s packet
			HAL_GPIO_WritePin(GPIOB, SPI_ETT_CS1_Pin, GPIO_PIN_SET);
			break;

		case 2:
			for (uint8_t rowNum = 0; rowNum < ETT_SEG_ROW_NUM; rowNum++){
				do{
					HAL_GPIO_WritePin(GPIOB, SPI_ETT_CS2_Pin, GPIO_PIN_RESET);
					HAL_SPI_Receive(&hspi1, (uint8_t *)&segResBuf, 1, 0xfff);
					HAL_GPIO_WritePin(GPIOB, SPI_ETT_CS2_Pin, GPIO_PIN_SET);
				} while( ((segResBuf >> 8) != 0) && timeout-- );

				if (timeout == 0)
					return;

				ettMatrix[rowNum+ETT_SEG_ROW_NUM] |= segResBuf;
			}
			HAL_GPIO_WritePin(GPIOB, SPI_ETT_CS2_Pin, GPIO_PIN_RESET);
			HAL_SPI_Receive(&hspi1, (uint8_t *)&segResBuf, 1, 0xfff);	// read 6th zero`s packet
			HAL_GPIO_WritePin(GPIOB, SPI_ETT_CS2_Pin, GPIO_PIN_SET);
			break;

		case 3:
			for (uint8_t rowNum = 0; rowNum < ETT_SEG_ROW_NUM; rowNum++){
				do{
					HAL_GPIO_WritePin(GPIOB, SPI_ETT_CS3_Pin, GPIO_PIN_RESET);
					HAL_SPI_Receive(&hspi1, (uint8_t *)&segResBuf, 1, 0xfff);
					HAL_GPIO_WritePin(GPIOB, SPI_ETT_CS3_Pin, GPIO_PIN_SET);
				} while( ((segResBuf >> 8) != 0) && timeout-- );

				if (timeout == 0)
					return;

				ettMatrix[rowNum] |= segResBuf << 7;
			}
			HAL_GPIO_WritePin(GPIOB, SPI_ETT_CS3_Pin, GPIO_PIN_RESET);
			HAL_SPI_Receive(&hspi1, (uint8_t *)&segResBuf, 1, 0xfff);	// read 6th zero`s packet
			HAL_GPIO_WritePin(GPIOB, SPI_ETT_CS3_Pin, GPIO_PIN_SET);
			break;

		case 4:
			for (uint8_t rowNum = 0; rowNum < ETT_SEG_ROW_NUM; rowNum++){
				do{
					HAL_GPIO_WritePin(GPIOB, SPI_ETT_CS4_Pin, GPIO_PIN_RESET);
					HAL_SPI_Receive(&hspi1, (uint8_t *)&segResBuf, 1, 0xfff);
					HAL_GPIO_WritePin(GPIOB, SPI_ETT_CS4_Pin, GPIO_PIN_SET);
				} while( ((segResBuf >> 8) != 0) && timeout-- );

				if (timeout == 0)
					return;

				ettMatrix[rowNum+ETT_SEG_ROW_NUM] |= segResBuf << 7;
			}
			HAL_GPIO_WritePin(GPIOB, SPI_ETT_CS4_Pin, GPIO_PIN_RESET);
			HAL_SPI_Receive(&hspi1, (uint8_t *)&segResBuf, 1, 0xfff);	// read 6th zero`s packet
			HAL_GPIO_WritePin(GPIOB, SPI_ETT_CS4_Pin, GPIO_PIN_SET);
			break;

		default:
			uartprintf("Wrong ETT segment checking");

	}

	HAL_GPIO_WritePin(GPIOB, SPI_ETT_CS1_Pin|SPI_ETT_CS2_Pin|SPI_ETT_CS3_Pin|SPI_ETT_CS4_Pin, GPIO_PIN_SET);
}



/**
  * @brief SPI1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_SPI1_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

	/* Peripheral clock enable */
	__HAL_RCC_SPI1_CLK_ENABLE();

	__HAL_RCC_GPIOA_CLK_ENABLE();
	/**SPI1 GPIO Configuration
	PA5     ------> SPI1_SCK
	PA6     ------> SPI1_MISO
	PA7     ------> SPI1_MOSI
	*/
	GPIO_InitStruct.Pin = GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7;
	GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
	GPIO_InitStruct.Alternate = GPIO_AF5_SPI1;
	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  __HAL_RCC_GPIOB_CLK_ENABLE();
  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, SPI_ETT_CS1_Pin|SPI_ETT_CS2_Pin|SPI_ETT_CS3_Pin|SPI_ETT_CS4_Pin, GPIO_PIN_SET);

  /*Configure GPIO pins : SPI_ETT_CS2_Pin SPI_ETT_CS3_Pin SPI_ETT_CS4_Pin SPI_ETT_CS_Pin */
  GPIO_InitStruct.Pin = SPI_ETT_CS1_Pin|SPI_ETT_CS2_Pin|SPI_ETT_CS3_Pin|SPI_ETT_CS4_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /* SPI1 parameter configuration*/
  hspi1.Instance = SPI1;
  hspi1.Init.Mode = SPI_MODE_MASTER;
  hspi1.Init.Direction = SPI_DIRECTION_2LINES;
  hspi1.Init.DataSize = SPI_DATASIZE_16BIT;
  hspi1.Init.CLKPolarity = SPI_POLARITY_LOW;
  hspi1.Init.CLKPhase = SPI_PHASE_1EDGE;
  hspi1.Init.NSS = SPI_NSS_SOFT;
  hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_256;
  hspi1.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi1.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi1.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi1.Init.CRCPolynomial = 10;
  if (HAL_SPI_Init(&hspi1) != HAL_OK)
  {
    Error_Handler();
  }

}


/**
* @brief SPI MSP De-Initialization
* This function freeze the hardware resources used in this example
* @param hspi: SPI handle pointer
* @retval None
*/
void HAL_SPI_MspDeInit(SPI_HandleTypeDef* hspi)
{
  if(hspi->Instance==SPI1)
  {
    /* Peripheral clock disable */
    __HAL_RCC_SPI1_CLK_DISABLE();

    /**SPI1 GPIO Configuration
    PA5     ------> SPI1_SCK
    PA6     ------> SPI1_MISO
    PA7     ------> SPI1_MOSI
    */
    HAL_GPIO_DeInit(GPIOA, GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7);

  }
  else if(hspi->Instance==SPI2)
  {
    /* Peripheral clock disable */
    __HAL_RCC_SPI2_CLK_DISABLE();

    /**SPI2 GPIO Configuration
    PC2     ------> SPI2_MISO
    PB10     ------> SPI2_SCK
    */
    HAL_GPIO_DeInit(GPIOC, GPIO_PIN_2);

    HAL_GPIO_DeInit(GPIOB, GPIO_PIN_10);

  }

}
