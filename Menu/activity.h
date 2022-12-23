
#ifndef __ACTIVITY_H
#define __ACTIVITY_H

#define ACTIVITY_NMBR 5

typedef enum Activity{
	MENU_ACTIVITY,
	TEST_ACTIVITY,
	TEST_CONFIG_ACTIVITY,
	TIME_ACTIVITY,
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
	UPDATE_CMD
} Command_t;

typedef void (*ActivityViewUpdateCb_t)(Command_t cmd);

void ActivityInit(void);
void ActivityViewUpdate(Command_t cmd);
void ActivityChange(Activity_t newActivity);

#endif	//__ACTIVITY_H