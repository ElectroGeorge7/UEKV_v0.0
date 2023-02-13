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
#include "activity.h"

#include "buttons_hardware.h"

#include "leds_matrix.h"
#include "result_check.h"
#include "ts_spi.h"
#include "MAX6675.h"

#include "LCD1602.h"

extern osMessageQueueId_t eventQueueHandler;
extern uint16_t ledsBitMatrix[];

void ControlTask(void *argument){
	uint8_t tempVal = 0;
	char tempStr[8] = {0};


	osStatus_t res;
	Event_t msg;

	activity_init();

	leds_matrix_init();
	ledsBitMatrix[0] = 0xFFFF;
	ledsBitMatrix[1] = 0xFFFF;
	ledsBitMatrix[2] = 0xFFFF;
	ledsBitMatrix[3] = 0xFFFF;
	ledsBitMatrix[4] = 0xFFFF;
	ledsBitMatrix[5] = 0xFFFF;
	ledsBitMatrix[6] = 0xFFFF;
	ledsBitMatrix[7] = 0xFFFF;
	ledsBitMatrix[8] = 0xFFFF;
	ledsBitMatrix[9] = 0xFFFF;
	leds_matrix_show_result();
	HAL_Delay(1000);
	leds_matrix_clear();

	//result_check_init();

  for(;;)
  {
	res = osMessageQueueGet(eventQueueHandler, &msg, NULL, osWaitForever);

	if( res == osOK )
	{
		if ( msg.event == BUTTON_UP_PRESS_EVENT )
			activity_cmd_execute(UP_CMD, NULL);
		else if ( msg.event == BUTTON_DOWN_PRESS_EVENT )
			activity_cmd_execute(DOWN_CMD, NULL);
		else if ( msg.event == BUTTON_RIGHT_PRESS_EVENT )
			activity_cmd_execute(SELECT_CMD, NULL);
		else if ( msg.event == BUTTON_LEFT_PRESS_EVENT )
			activity_cmd_execute(BACK_CMD, NULL);
		else if ( msg.event == TERMINAL_СMD_RECEIVED_EVENT )
			// @todo preprocessing to identify common commands for all activities
			activity_cmd_execute(TERMINAL_CMD, &(msg.eventStr));
		else
			activity_cmd_execute(START_CMD, NULL);
	}

	//usbprintf("ControlTask");

    osDelay(1000);
  }
}
