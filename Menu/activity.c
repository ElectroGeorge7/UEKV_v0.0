
#include "activity.h"

#include "menu.h"
#include "date_time_activity.h"
#include "test_activity.h"
#include "lps_activity.h"

static Activity_t gActivity = MENU_ACTIVITY;

static ActivityViewUpdateCb_t activityViewUpdateCbArray[ACTIVITY_NMBR] = {0};

void activity_init(void){
	MenuRegisterActivityCb(&(activityViewUpdateCbArray[MENU_ACTIVITY]));
	//TestRegisterActivityCb(&ActivityViewUpdateCbArray[TEST_ACTIVITY]);
	test_reg_activity_cb(&activityViewUpdateCbArray[TEST_ACTIVITY]);
	date_time_reg_activity_cb(&activityViewUpdateCbArray[DATE_TIME_ACTIVITY]);
	lps_reg_activity_cb(&activityViewUpdateCbArray[LPS_ACTIVITY]);
	//NetworkRegisterActivityCb(&ActivityViewUpdateCbArray[NETWORK_ACTIVITY]);
	
	activity_cmd_execute(START_CMD, NULL);
};

void activity_cmd_execute(Command_t cmd, uint8_t *data){
	
	ActivityViewUpdateCb_t activityViewFunc = 0;
	
	if (NULL_ACTIVITY != gActivity){
		activityViewFunc = activityViewUpdateCbArray[gActivity];
		activityViewFunc(cmd, data);
	}else{
		gActivity = MENU_ACTIVITY;
		activity_cmd_execute(START_CMD, NULL);
	}
	
};


void activity_change(Activity_t newActivity){
	
	if(NULL_ACTIVITY != newActivity){
		gActivity = newActivity;
		activity_cmd_execute(UPDATE_CMD, NULL);
	}
}

