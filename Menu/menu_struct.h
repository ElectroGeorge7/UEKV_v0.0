
#ifndef __MENU_STRUCT_H
#define __MENU_STRUCT_H

#include "activity.h"

/*
* Отличная реализация меню, которую взял в качестве примера
* http://easyelectronics.ru/organizaciya-drevovidnogo-menyu.html
*/
typedef struct
{
	void *Previous;
	void *Next;
	void *Parent;
	void *Child;
	//void (*ItemFunc)(void);
	Activity_t ChildActivity;
	char *ItemName;
} MenuItem_t;

MenuItem_t NullMenuItem = { (void*)0, (void*)0, (void*)0, (void*)0, NULL_ACTIVITY , {0x00}};
#define NULL_MENU_ITEM	NullMenuItem


#define MAKE_MENU_ITEM(ItemName, Previous, Next, Parent, Child, Func, ItemStrName) \
extern  MenuItem_t Previous; \
extern  MenuItem_t Next; 		\
extern  MenuItem_t Parent;		\
extern  MenuItem_t Child;		\
MenuItem_t ItemName = {&Previous, &Next, &Parent, &Child, Func, ItemStrName}


MAKE_MENU_ITEM(Tests, NULL_MENU_ITEM, Time, NULL_MENU_ITEM, NULL_MENU_ITEM, TEST_ACTIVITY, "Испытания");
MAKE_MENU_ITEM(Time, Tests, Lps, NULL_MENU_ITEM, NULL_MENU_ITEM, DATE_TIME_ACTIVITY, "Дата и время");
MAKE_MENU_ITEM(Lps, Time, Temp, NULL_MENU_ITEM, NULL_MENU_ITEM, LPS_ACTIVITY, "ИП");
MAKE_MENU_ITEM(Temp, Lps, NULL_MENU_ITEM, NULL_MENU_ITEM, NULL_MENU_ITEM, TEMP_ACTIVITY, "Температура");

// display the zero menu level
static MenuItem_t *firstViewItem = &Tests;
static MenuItem_t *lastViewItem = &Time;
static MenuItem_t *currentItem = &Tests;

#endif //_MENU_STRUCT_H
