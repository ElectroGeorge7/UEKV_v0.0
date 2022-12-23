
#ifndef MENU_H_
#define MENU_H_

#include "stm32f4xx_hal.h"

#include "activity.h"

#include <stdint.h>

#define	MENU_LEVEL	10		//максимальный уровень вложенности меню
#define	SUB_MENU	7		//максимальное количество подменю в одном меню

typedef enum MenuAction {
	MENU_UPDATE,		
	MENU_START,
	MENU_ITEM_UP,
	MENU_ITEM_DOWN,
	MENU_ITEM_LEFT,
	MENU_ITEM_RIGHT,
	MENU_ITEM_SELECT,
	MENU_ITEM_BACK
} MenuAction_t;



typedef struct
{
	uint8_t* MenuName;					//Заголовок меню
	uint32_t MenuLevel;					//Уровень меню (0 - самый верхний и т. д.)
	uint32_t NumberOfSubMenu;			//Количество подменю
	uint32_t CurrentAssignSubMenu;		//Текущее выделенное подменю (0 - не выделено ни одно подменю, 1 - выделено первое подменю и т.д.)
	uint32_t BottomMenu;				//Признак последнего меню (следующего уровня подменю нет), 1 - последнее подменю, 0 - не последнее.
	uint32_t ParentMenu;				//Родительское меню, из которого попали в текущее меню
	uint8_t* SubMenuName[SUB_MENU];		//Заголовок подменю 1
} Menu_t;


HAL_StatusTypeDef menu_init(void);
HAL_StatusTypeDef menu_view_update(Command_t menuAction);

void DrawMenu(Menu_t*);
void DrawFunction(Menu_t*);
void MenuDisplayUpdate(MenuAction_t menuAction);

void MenuViewUpdate(Command_t menuAction);
void MenuRegisterActivityCb(ActivityViewUpdateCb_t *pMenuViewUpdateCb);

#endif	//MENU_H_
