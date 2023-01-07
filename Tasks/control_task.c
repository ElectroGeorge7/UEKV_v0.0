/*
 * control_task.c
 *
 *  Created on: Jan 7, 2023
 *      Author: George
 */

#include "control_task.h"

#include "main.h"
#include "terminal.h"

#include "cmsis_os2.h"

#include "menu.h"

#include "buttons_hardware.h"

#include "leds_matrix.h"
#include "result_check.h"
#include "ts_spi.h"
#include "MAX6675.h"

#include "LCD1602.h"

extern osMessageQueueId_t eventQueueHandler;

void ControlTask(void *argument){
	uint8_t tempVal = 0;
	char tempStr[8] = {0};

	osStatus_t res;
	uint8_t event;

	leds_matrix_init();
	result_check_init();

  for(;;)
  {
	res = osMessageQueueGet(eventQueueHandler, &event, NULL, osWaitForever);

	if( res == osOK )
	{
		if ( event == BUTTON_UP_PRESS_EVENT )
			menu_view_update(DOWN_CMD);
		else if ( event == BUTTON_DOWN_PRESS_EVENT )
			menu_view_update(DOWN_CMD);
		else if ( event == BUTTON_RIGHT_PRESS_EVENT )
			menu_view_update(SELECT_CMD);
		else if ( event == BUTTON_LEFT_PRESS_EVENT )
			menu_view_update(BACK_CMD);
		else
			menu_view_update(START_CMD);
	}

    osDelay(1000);
  }
}
