#include "resource.h"


#define ID_NEW 1
#define ID_OPEN 2
#define ID_SAVE 3
#define ID_PRINT 4
#define ID_CLOSE 5
#define ID_PEN 6
#define ID_BRUSH 7
#define ID_TEXT 8
#define ID_COLOR 9
#define ID_WIDTH1 10
#define ID_WIDTH2 11
#define ID_WIDTH3 12
#define ID_WIDTH4 13
#define ID_WIDTH5 14
#define ID_WIDTH6 15
#define ID_LINE 16
#define ID_BROKEN 17
#define ID_RECTANGLE 18
#define ID_ELLIPSE 19
#define ID_POLYGON 20
#define ID_ABOUT 21
#define ID_CLEAR 22
#define ID_CANCEL 23

void SetMenuBitmaps(HMENU hMenu,HINSTANCE hInst,char BitmapName,char command,LPCSTR Name)
{
	AppendMenu(hMenu, MF_STRING, (UINT)command, Name);
	HBITMAP hBitmap=LoadBitmap(hInst,MAKEINTRESOURCE(BitmapName));
	SetMenuItemBitmaps(hMenu,(UINT)command,MF_BYCOMMAND,hBitmap,hBitmap);
}

HMENU CreateMyMenu(HWND hMainWnd,HINSTANCE hInst)
{
	 HMENU MainMenu = CreateMenu();
	 HMENU hPopupMenu_File,hPopupMenu_Tools,hPopupMenu_Color,hPopupMenu_Width,hPopupMenu_Figures;
 
	 hPopupMenu_File = CreatePopupMenu();
     AppendMenu(MainMenu, MF_STRING | MF_POPUP, (UINT)hPopupMenu_File, "Файл"); 
     {
		 SetMenuBitmaps(hPopupMenu_File,hInst,IDB_BITMAP5,ID_NEW,"Новый\t Ctrl+N");
		 SetMenuBitmaps(hPopupMenu_File,hInst,IDB_BITMAP6,ID_OPEN,"Открыть\t Ctrl+O");
		 SetMenuBitmaps(hPopupMenu_File,hInst,IDB_BITMAP7,ID_SAVE,"Сохранить\t Ctrl+S");
		 SetMenuBitmaps(hPopupMenu_File,hInst,IDB_BITMAP8,ID_PRINT,"Печать\t Ctrl+P");
         AppendMenu(hPopupMenu_File, MF_SEPARATOR, 0, "");
		 SetMenuBitmaps(hPopupMenu_File,hInst,IDB_BITMAP9,ID_CLOSE,"Выход\t Ctrl+E");
     }
	 hPopupMenu_Tools = CreatePopupMenu();
	 hPopupMenu_Color = CreatePopupMenu();
	 hPopupMenu_Width = CreatePopupMenu();
	 hPopupMenu_Figures = CreatePopupMenu();

	 AppendMenu(MainMenu, MF_STRING | MF_POPUP, (UINT)hPopupMenu_Tools, "Инструменты");  
	 {
		 SetMenuBitmaps(hPopupMenu_Tools,hInst,IDB_BITMAP12,ID_PEN,"Карандаш");
		 SetMenuBitmaps(hPopupMenu_Tools,hInst,IDB_BITMAP13,ID_TEXT,"Текст");
		 AppendMenu(hPopupMenu_Tools, MF_STRING, (UINT)ID_CLEAR, "Ластик");
		 AppendMenu(hPopupMenu_Tools, MF_SEPARATOR, 0, "");
		 SetMenuBitmaps(hPopupMenu_Tools,hInst,IDB_BITMAP10,ID_BRUSH,"Заливка");
		 AppendMenu(hPopupMenu_Tools, MF_STRING, (UINT)ID_COLOR, "Цвет");
		 AppendMenu(hPopupMenu_Tools, MF_SEPARATOR, 0, "");
		 AppendMenu(hPopupMenu_Tools, MF_STRING|MF_POPUP, (UINT)hPopupMenu_Width, "Толщина");
		 {
			 AppendMenu(hPopupMenu_Width, MF_STRING, (UINT)ID_WIDTH1, "1");
			 AppendMenu(hPopupMenu_Width, MF_STRING, (UINT)ID_WIDTH2, "2");
			 AppendMenu(hPopupMenu_Width, MF_STRING, (UINT)ID_WIDTH3, "3");
			 AppendMenu(hPopupMenu_Width, MF_STRING, (UINT)ID_WIDTH4, "4");
			 AppendMenu(hPopupMenu_Width, MF_STRING, (UINT)ID_WIDTH5, "5");
			 AppendMenu(hPopupMenu_Width, MF_STRING, (UINT)ID_WIDTH6, "6");
		 }
		 AppendMenu(hPopupMenu_Tools, MF_STRING, (UINT)ID_CANCEL, "Отмена");
	 }

	 	 AppendMenu(MainMenu, MF_STRING | MF_POPUP, (UINT)hPopupMenu_Figures, "Фигуры");  
	 {
         AppendMenu(hPopupMenu_Figures, MF_STRING, (UINT)ID_LINE, "Прямая");
         AppendMenu(hPopupMenu_Figures, MF_STRING, (UINT)ID_BROKEN, "Ломаная");
         AppendMenu(hPopupMenu_Figures, MF_STRING, (UINT)ID_RECTANGLE, "Прямоугольник");
         AppendMenu(hPopupMenu_Figures, MF_STRING, (UINT)ID_ELLIPSE, "Эллипс");
         AppendMenu(hPopupMenu_Figures, MF_STRING, (UINT)ID_POLYGON, "Многоугольник");
	 }
     AppendMenu(MainMenu, MF_STRING | MF_POPUP, (UINT)ID_ABOUT, "О программе");   
     
     SetMenu(hMainWnd, MainMenu);
	 return MainMenu;
}