/*
**	Функции для работы с меню, отображаемым на ЖК индикаторе.
*/

#include "menu.h"
#include "menu_struct.h"
#include "activity.h"

#include "main.h"
#include "LCD1602.h"

Menu_t *CurrentMenu;	//указатель на текущее меню
Menu_t TopMenu = {"--- Главное меню ---",0,2,0,0,0,{"Отображаемые символы","Описание"}};
Menu_t Submenu1 = {"Отображаемые символы",1,0,0,1,0};
Menu_t Submenu2 = {"Описание",1,0,0,1,0};

#ifdef DEB
//Функция отображения меню на LCD-диссплее
//Параметры: Menu - указатель на меню, которое необходимо отобразить на LCD
void DrawMenu(_Menu* Menu)
{	
	uint32_t i;

	PrintConstText(Menu->MenuName,0,(64-(StrLen(Menu->MenuName)*FONT_WIDTH/2)),0);
	for(i=1;i<(Menu->NumberOfSubMenu+1);i++)
	{
		if(Menu->CurrentAssignSubMenu != i)	PrintConstText(Menu->SubMenuName[i-1],i,(64-(StrLen(Menu->SubMenuName[i-1])*FONT_WIDTH/2)),0);
		else PrintConstText(Menu->SubMenuName[i-1],i,(64-(StrLen(Menu->SubMenuName[i-1])*FONT_WIDTH/2)),1);
	}
}

//Функция вывода действий на дисплей
//Параметры: Menu - указатель на меню, при выборе которого надо выполнить какие-либо действия
void DrawFunction(_Menu* Menu)
{
	uint32_t Symbol,i,j;	
	uint8_t* Text1={"Демоплата EVAL22.0B"};
	uint8_t* Text2={"для 32-разрядного "};
	uint8_t* Text3={"микрконтроллера "};
	uint8_t* Text4={"1986ВЕ1"};

	if(Menu == &Submenu1)
	{
		Symbol = 0;
		for(i=0;i<8;i++)
		{
			SetPage(i,First);
			SetPage(i,Second);
			for(j=0;j<126;j++)
			{
				WriteByte(Font_6x8_Data[Symbol++],j);
				if(Symbol == 42) Symbol = 192;
				if(Symbol == 762) Symbol = 1008;
				if(Symbol == 1014) Symbol = 1104;
				if(Symbol == 1110) Symbol = 1152;
			}
		}	
	}
	if(CurrentMenu == &Submenu2)
	{
		ClearLCD();
		PrintConstText(Text1,2,(64-(19*FONT_WIDTH/2)),0);
		PrintConstText(Text2,3,(64-(17*FONT_WIDTH/2)),0);
		PrintConstText(Text3,4,(64-(15*FONT_WIDTH/2)),0);
		PrintConstText(Text4,5,(64-(7*FONT_WIDTH/2)),0);
	}
}

void MenuDisplayUpdate(MenuAction_t menuAction){
	uint8_t *tempStr;
	
	if ( menuAction == MENU_START ){
		ClearLCD();
		tempStr = "АО `ПКК Миландр`";
		PrintConstText(tempStr, 0, (64-(StrLen(tempStr)*6/2)), 0);
		tempStr = "Универсальный";
		PrintConstText(tempStr, 2, (64-(StrLen(tempStr)*6/2)), 0);
		tempStr = "эквивалент";
		PrintConstText(tempStr, 3, (64-(StrLen(tempStr)*6/2)), 0);
		tempStr = "`EQV-V1`";
		PrintConstText(tempStr, 4, (64-(StrLen(tempStr)*6/2)), 0);
		tempStr = "2021";
		PrintConstText(tempStr, 7, (64-(StrLen(tempStr)*6/2)), 0);
	}
}
#endif

extern MenuItem_t *firstViewItem;
extern MenuItem_t *lastViewItem;
extern MenuItem_t *currentItem;
static uint8_t currentMenuListRowNmbr = 0; 

void menu_list_display(void){
	MenuItem_t *tempMenuItem = 0;
	uint8_t i = 1;
	uint8_t curPos = 0;
	
	tempMenuItem = firstViewItem;
	LCD_Clear();
	LCD_CursorOnOff(0);
	
	do {
			LCD_SetCursor( 0, i-1 );
			LCD_PrintString((uint8_t*)(tempMenuItem->ItemName));
			if ( tempMenuItem == currentItem )
				curPos = ( tempMenuItem == currentItem ) ? i-1 : 0;
			tempMenuItem = tempMenuItem->Next;
	} while(((i++) < 2) && ((MenuItem_t*)tempMenuItem != &NULL_MENU_ITEM));
	
	LCD_SetCursor( 15, curPos );
	LCD_CursorOnOff(1);
}


HAL_StatusTypeDef menu_init(void){
	LCD_Clear();
	LCD_SetCursor( 0, 0 );
	LCD_PrintString("АО `ПКК Миландр`");
	LCD_SetCursor( 4, 1 );
	LCD_PrintString("UEKV-V1.0");

	HAL_Delay(2000);

	menu_list_display();
	return HAL_OK;
}


HAL_StatusTypeDef menu_view_update(Command_t menuAction){
	uint8_t i = 0;	
		
	switch (menuAction){
		case START_CMD:
			menu_init();
			break;
		case UP_CMD:
			if ( currentItem->Previous != &NULL_MENU_ITEM ){
				if ( currentItem != firstViewItem ){
					currentItem = currentItem->Previous;
					LCD_SetCursor( 15, --currentMenuListRowNmbr%2 );
				}else{
					currentItem = currentItem->Previous;
					currentMenuListRowNmbr--;
					for ( i=0; (i < 2) && (firstViewItem->Previous != &NULL_MENU_ITEM); i++)
						firstViewItem = firstViewItem->Previous;
					menu_list_display();
				}
			}
			break;
		case DOWN_CMD:
			if ( currentItem->Next != &NULL_MENU_ITEM){
				if ( currentItem != lastViewItem ){
					currentItem = currentItem->Next;
					LCD_SetCursor( 15, ++currentMenuListRowNmbr%2 );
				}else{
					currentItem = currentItem->Next;
					currentMenuListRowNmbr++;
					firstViewItem = lastViewItem->Next;
					lastViewItem = firstViewItem->Next;
					menu_list_display();
				}
			}
			break;
		case SELECT_CMD:
			if ( currentItem->Child != &NULL_MENU_ITEM ){
				firstViewItem = currentItem->Child;
				lastViewItem = firstViewItem->Next;
				currentItem = firstViewItem;
				currentMenuListRowNmbr = 0;
				menu_list_display();
			}else {
				if (currentItem->ChildActivity != NULL_ACTIVITY){
					//ActivityChange(currentItem->ChildActivity);
				}
			}
			break;
		case BACK_CMD:
			if ( currentItem->Parent != &NULL_MENU_ITEM ){
				firstViewItem = currentItem->Parent;
				currentItem = firstViewItem;
				currentMenuListRowNmbr = 0;
				menu_list_display();
			}
			break;
		case UPDATE_CMD:
			menu_list_display();
		case LEFT_CMD:
		case RIGHT_CMD:
	  default:
			break;
	}

	return HAL_OK;
}


void MenuRegisterActivityCb(ActivityViewUpdateCb_t *pMenuViewUpdateCb){
	*pMenuViewUpdateCb = menu_view_update;
}

