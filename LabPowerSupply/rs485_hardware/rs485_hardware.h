/*
 * rs485_hardware.h
 *
 *  Created on: 5 нояб. 2022 г.
 *      Author: George
 */

#ifndef RS485_HARDWARE_H_
#define RS485_HARDWARE_H_

#include "stm32f4xx_hal.h"

HAL_StatusTypeDef rs485_init(void);
HAL_StatusTypeDef rs485_receive(uint8_t *rxData, uint16_t size);
HAL_StatusTypeDef rs485_transmit(uint8_t *cmd, uint16_t size);
HAL_StatusTypeDef rs485_transmit_w_respond(uint8_t *cmd, uint16_t txSize, uint8_t *respond, uint16_t rxSize);

#endif /* RS485_HARDWARE_H_ */
