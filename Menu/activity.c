
#include "activity.h"

#include "menu.h"
//#include "time.h"
//#include "network.h"

static Activity_t gActivity = NULL_ACTIVITY;

static ActivityViewUpdateCb_t ActivityViewUpdateCbArray[ACTIVITY_NMBR] = {0};

void ActivityInit(void){
	MenuRegisterActivityCb(&(ActivityViewUpdateCbArray[MENU_ACTIVITY]));
	TestRegisterActivityCb(&ActivityViewUpdateCbArray[TEST_ACTIVITY]);
	//TimeRegisterActivityCb(&ActivityViewUpdateCbArray[TIME_ACTIVITY]);
	//NetworkRegisterActivityCb(&ActivityViewUpdateCbArray[NETWORK_ACTIVITY]); 
	
	ActivityViewUpdate(START_CMD);
};

void ActivityViewUpdate(Command_t cmd){
	
	ActivityViewUpdateCb_t activityViewFunc = 0;
	
	if (NULL_ACTIVITY != gActivity){
		activityViewFunc = ActivityViewUpdateCbArray[gActivity];
		activityViewFunc(cmd);
	}else{
		gActivity = MENU_ACTIVITY;
		ActivityViewUpdate(START_CMD);
	}
	
};


void ActivityChange(Activity_t newActivity){
	
	if(NULL_ACTIVITY != newActivity){
		gActivity = newActivity;
		ActivityViewUpdate(UPDATE_CMD);
	}
}

