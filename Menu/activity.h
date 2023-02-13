
#ifndef ACTIVITY_H__
#define ACTIVITY_H__

#include "stm32f4xx_hal.h"

#define ACTIVITY_NMBR 5

typedef enum Activity{
	MENU_ACTIVITY,
	TEST_ACTIVITY,
	DATE_TIME_ACTIVITY,
	NETWORK_ACTIVITY,
	NULL_ACTIVITY
} Activity_t;

typedef enum Command {
	UP_CMD,
	DOWN_CMD,
	LEFT_CMD,
	RIGHT_CMD,
	SELECT_CMD, 
	BACK_CMD,
	START_CMD,
	UPDATE_CMD,
	TERMINAL_CMD
} Command_t;

typedef void (*ActivityViewUpdateCb_t)(Command_t cmd, uint8_t *data);

void activity_init(void);
void activity_cmd_execute(Command_t cmd, uint8_t *data);
void activity_change(Activity_t newActivity);

#endif	//ACTIVITY_H__
