/*
**	Функции для работы с меню, отображаемым на ЖК индикаторе.
*/

#include "menu.h"
#include "menu_struct.h"
#include "activity.h"

#include "main.h"
#include "terminal.h"
#include "LCD1602.h"

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
	LCD_PrintString("UEKV_V3");

	HAL_Delay(2000);

	menu_list_display();
	return HAL_OK;
}


HAL_StatusTypeDef menu_view_update(Command_t menuAction, uint8_t *data){
	uint8_t i = 0;	

	uartprintf("Menu action: %d", menuAction);
	uartprintf("currentItem: %s", currentItem->ItemName);
	uartprintf("firstViewItem: %s", firstViewItem->ItemName);
	uartprintf("lastViewItem: %s", lastViewItem->ItemName);
		
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
					currentItem = lastViewItem = currentItem->Previous;
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
					activity_change(currentItem->ChildActivity);
					activity_cmd_execute(SELECT_CMD, NULL);
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


void menu_reg_activity_cb(ActivityViewUpdateCb_t *pMenuViewUpdateCb){
	*pMenuViewUpdateCb = menu_view_update;
}

