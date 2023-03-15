/*
 * rs485_hardware.h
 *
 *  Created on: 5 нояб. 2022 г.
 *      Author: George
 */

#ifndef RS485_HARDWARE_H_
#define RS485_HARDWARE_H_

#include "stm32f4xx_hal.h"

typedef void (*Rs485RespondHandler_t)(void);

HAL_StatusTypeDef rs485_init(void);
void rs485_reg_respond_handler(Rs485RespondHandler_t hFunc);
HAL_StatusTypeDef rs485_tx(uint8_t *cmd, uint16_t size, uint8_t repeat, uint8_t delay_ms);
HAL_StatusTypeDef rs485_rx_start(uint8_t *rxData, uint16_t size, uint8_t repeat);
HAL_StatusTypeDef rs485_rx_stop(void);


#endif /* RS485_HARDWARE_H_ */
