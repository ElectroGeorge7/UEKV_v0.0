/*
 * status_led.h
 *
 *  Created on: 2 сент. 2023 г.
 *      Author: George
 */

#ifndef STATUS_LEDS_H_
#define STATUS_LEDS_H_

#include "stm32f4xx_hal.h"

#define LED_ERROR_Pin GPIO_PIN_13
#define LED_ERROR_GPIO_Port GPIOC
#define LED_PROCESS_Pin GPIO_PIN_12
#define LED_PROCESS_GPIO_Port GPIOC

void status_leds_init(void);
void status_leds_set(uint16_t leds);
void status_leds_reset(uint16_t leds);
void status_leds_toggle(uint16_t leds);

#endif /* STATUS_LEDS_H_ */
