
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

// Start Menu
// Главное меню - Тесты
MAKE_MENU_ITEM(Tests, NULL_MENU_ITEM, Time, NULL_MENU_ITEM, NULL_MENU_ITEM, TEST_ACTIVITY, "Испытания");
		//Подменю ур.1 - Безотказность
		MAKE_MENU_ITEM(Reliability, NULL_MENU_ITEM, Ett, Tests, NULL_MENU_ITEM, TEST_ACTIVITY, "Безотказность");
		MAKE_MENU_ITEM(Ett, Reliability, Item13, Tests, NULL_MENU_ITEM, TEST_ACTIVITY, "ЭТТ");
		MAKE_MENU_ITEM(Item13, Ett, Item14, Tests, NULL_MENU_ITEM, NULL_ACTIVITY, "Пункт13");
		MAKE_MENU_ITEM(Item14, Item13, NULL_MENU_ITEM, Tests, NULL_MENU_ITEM, NULL_ACTIVITY, "Пункт14");
		//MAKE_MENU_ITEM(TestConfig, Item14, NULL_MENU_ITEM, Tests, NULL_MENU_ITEM, NULL_FUNC, "Конфигурация");
//MAKE_MENU_ITEM(TestConfig, Tests, Time, NULL_MENU_ITEM, NULL_MENU_ITEM, NULL_ACTIVITY, "Конфигурация"); // способ общения с платой тестировния и количество COL и ROW
// Главное меню - Дата и время
MAKE_MENU_ITEM(Time, Tests, Network, NULL_MENU_ITEM, NULL_MENU_ITEM, DATE_TIME_ACTIVITY, "Дата и время");

// Главное меню - Сеть
MAKE_MENU_ITEM(Network, Time, Item4, NULL_MENU_ITEM, NULL_MENU_ITEM, NULL_ACTIVITY, "Сеть");


MAKE_MENU_ITEM(Item4, Network, Item5, NULL_MENU_ITEM, NULL_MENU_ITEM, NULL_ACTIVITY, "Пункт4");
MAKE_MENU_ITEM(Item5, Item4, Item6, NULL_MENU_ITEM, NULL_MENU_ITEM, NULL_ACTIVITY, "Пункт5");
MAKE_MENU_ITEM(Item6, Item5, Item7, NULL_MENU_ITEM, NULL_MENU_ITEM, NULL_ACTIVITY, "Пункт6");
MAKE_MENU_ITEM(Item7, Item6, Item8, NULL_MENU_ITEM, NULL_MENU_ITEM, NULL_ACTIVITY, "Пункт7");
MAKE_MENU_ITEM(Item8, Item7, Item9, NULL_MENU_ITEM, NULL_MENU_ITEM, NULL_ACTIVITY, "Пункт8");
MAKE_MENU_ITEM(Item9, Item8, Item10, NULL_MENU_ITEM, NULL_MENU_ITEM, NULL_ACTIVITY, "Пункт9");
MAKE_MENU_ITEM(Item10, Item9, NULL_MENU_ITEM, NULL_MENU_ITEM, NULL_MENU_ITEM, NULL_ACTIVITY, "Пункт10");

// display the zero menu level
static MenuItem_t *firstViewItem = &Tests;
static MenuItem_t *lastViewItem = &Time;
static MenuItem_t *currentItem = &Tests;

#endif //_MENU_STRUCT_H
