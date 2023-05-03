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
#include "reliability.h"

#include "leds_matrix.h"
#include "result_check.h"
#include "ts_spi.h"
#include "MAX6675.h"

#include "LCD1602.h"

extern osMessageQueueId_t eventQueueHandler;
extern osEventFlagsId_t testEvents;

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


	// if it was reset during the test process than resume the test
	if ( bkp_read_data(UEKV_LAST_STATE_REG) == UEKV_TEST_STATE ){
		osEventFlagsSet(testEvents, LPS_LIST_UDATE_START);
		osThreadYield();
		osDelay(1);

		// go to test menu and resume the test
		activity_cmd_execute(SELECT_CMD, NULL);
		activity_cmd_execute(SELECT_CMD, NULL);
	}

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
		else if ( msg.event == ACTIVITY_UPDATE_EVENT )
			activity_cmd_execute(UPDATE_CMD, &(msg.eventStr));
		else if ( msg.event == TERMINAL_CMD_RECEIVED_EVENT )
			// @todo preprocessing to identify common commands for all activities
			activity_cmd_execute(TERMINAL_CMD, &(msg.eventStr));
		else
			activity_cmd_execute(START_CMD, NULL);
	}

	//usbprintf("ControlTask");

    osDelay(1000);
  }
}
