/*
 * ts_spi.h
 *
 *  Created on: 30 окт. 2022 г.
 *      Author: George
 */

#ifndef TS_HARDWARE_TS_SPI_H_
#define TS_HARDWARE_TS_SPI_H_

#include "stm32f4xx_hal.h"

#define TEMP_SENS_CS1_Pin GPIO_PIN_0
#define TEMP_SENS_CS1_GPIO_Port GPIOC
#define TEMP_SENS_CS2_Pin GPIO_PIN_1
#define TEMP_SENS_CS2_GPIO_Port GPIOC

HAL_StatusTypeDef ts_spi_init(void);

#endif /* TS_HARDWARE_TS_SPI_H_ */
