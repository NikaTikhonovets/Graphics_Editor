#include <Windows.h>
#include <string>
#include "string.h"
#include "resource.h"
#include "afxres.h"
#include "Menu.h"
#include "Functions.h"


LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
BOOL CALLBACK DlgProc(HWND, UINT, WPARAM, LPARAM);

ATOM RegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX WndClsEx;

	WndClsEx.cbSize = sizeof(WNDCLASSEX); 
    WndClsEx.style = CS_HREDRAW | CS_VREDRAW;
    WndClsEx.lpfnWndProc = WndProc;
    WndClsEx.cbClsExtra = 0;
    WndClsEx.cbWndExtra = 0;
    WndClsEx.hInstance = hInstance;
	WndClsEx.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON1));
	WndClsEx.hIconSm =  WndClsEx.hIcon;
    WndClsEx.hCursor = LoadCursor(NULL, IDC_CROSS);
	WndClsEx.hbrBackground = (HBRUSH)(WHITE_BRUSH);
    WndClsEx.lpszMenuName = NULL;
    WndClsEx.lpszClassName = "Main_class";
	
	RegisterClassEx(&WndClsEx);

	return 0;
}

BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
	hMainWnd = CreateWindow(
		"Main_class",
		"Графический редактор",
        WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT,
		0,
        CW_USEDEFAULT,
		0,
		NULL,
		NULL,
		hInstance, 
		NULL
	);

    ShowWindow(hMainWnd, nCmdShow);
    UpdateWindow(hMainWnd);

	return TRUE;
}



int WINAPI WinMain(HINSTANCE hInstance,
    HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{ 
	MSG Msg;
	HACCEL hAccelTable;
	RegisterClass(hInstance);
    InitInstance(hInstance,nCmdShow);
	hInst=hInstance;
	CreateMyMenu(hMainWnd,hInst);
	hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDR_ACCELERATOR1));

	while (GetMessage(&Msg, NULL, 0, 0))
	{
		if (!TranslateAccelerator(Msg.hwnd, hAccelTable, &Msg))
		{
			TranslateMessage(&Msg);
			DispatchMessage(&Msg);
		}
	}
    return (int)Msg.wParam;
}




LRESULT CALLBACK WndProc(HWND hMainWnd, UINT message,
    WPARAM wParam, LPARAM lParam)
{
	hdc = GetDC(hMainWnd);
	HACCEL hAccelTable;

    switch (message) 
    {
	case WM_CREATE:
		hdcMeta=InitializeTempDC(hMainWnd,hdc);
		InitHDC();
		break;
	case WM_COMMAND:
        switch (LOWORD(wParam)) {
			/*case ID_PEN:
				SetCursor(LoadCursor(hInst,MAKEINTRESOURCE(IDC_HAND)));
				break;*/
			case ID_SAVE:
				InitStructFile("Save");
				if (GetSaveFileName(&openFileName) == TRUE)		
					SaveEnhFile();
				break;
			case ID_OPEN:
				InitStructFile("Open");
				if (GetOpenFileName(&openFileName) == TRUE)
					LoadEnhFile();
				break;
			case ID_PRINT:
				InitStructPrint();
				if (PrintDlg(&printDlg) == TRUE)
					PrintFile();
				break;
			case ID_CANCEL:
				StretchBlt(finalPicture,0,0,rect.right,rect.bottom,prevFinalPicture,0,0,rect.right,rect.bottom,SRCCOPY);
				StretchBlt(hdc,0,0,rect.right,rect.bottom,finalPicture,offsetX,offsetY,rect.right/scale,rect.bottom/scale,SRCCOPY);
				StretchBlt(hDrawingArea,0,0,rect.right,rect.bottom,finalPicture,0,0,rect.right,rect.bottom,SRCCOPY);
				break;
			case ID_COLOR:
				ChangeColor(ID_COLOR);
				break;
			case ID_BRUSH:
				ChangeColor(ID_BRUSH);
				break;
			case ID_WIDTH1:
			case ID_WIDTH2:
			case ID_WIDTH3:
			case ID_WIDTH4:
			case ID_WIDTH5:
			case ID_WIDTH6:
				widthPen = LOWORD(wParam) - ID_WIDTH1 + 1;
				DeleteObject(hPen);
				hPen = CreatePen(PS_SOLID, widthPen, currentColor);
				break;
			case ID_CLEAR:
				tool=ID_CLEAR;
				DeleteObject(hPen);
				hPen=CreatePen(PS_SOLID,6,RGB(255,255,255));
				break;
			case ID_ABOUT:
				DialogBoxParam(hInst,MAKEINTRESOURCE(IDD_DIALOG1),hMainWnd,DlgProc,0 );
				break;
			case ID_CLOSE:
				SendMessage(hMainWnd,WM_DESTROY,wParam,lParam);
				break;
			case ID_NEW:
				GetClientRect(hMainWnd,&rect);
				PatBlt(finalPicture,0,0,rect.right,rect.bottom,WHITENESS);
				PatBlt(prevFinalPicture,0,0,rect.right,rect.bottom,WHITENESS);
				PatBlt(hDrawingArea,0,0,rect.right,rect.bottom,WHITENESS);
				PatBlt(hdc,0,0,rect.right,rect.bottom,WHITENESS);
				break;
			default:
				tool = LOWORD(wParam);
				DeleteObject(hPen);
				hPen = CreatePen(PS_SOLID, widthPen, currentColor);
			}
			break;
	case WM_LBUTTONDOWN:
		StartPos=MAKEPOINTS(lParam);
		GetNewCoord(StartPos.x,StartPos.y,offsetX,offsetY,scale);
		break;
	case WM_LBUTTONUP:
		EndPos=MAKEPOINTS(lParam);
		GetNewCoord(EndPos.x,EndPos.y,offsetX,offsetY,scale);
		switch (tool)
		{
			case ID_ELLIPSE:
			case ID_RECTANGLE:
			case ID_LINE:
				Draw(hDrawingArea, tool);
				Draw(hdcMeta,tool);
				break;
			case ID_TEXT:
				StartPos = EndPos;
				string.clear();
				break;
			case ID_POLYGON:
			case ID_BROKEN:
				AddPoint();
				Draw(hDrawingArea, tool);
				Draw(hdcMeta,tool);
				break;
		}
		StretchBlt(hdc,0,0,rect.right,rect.bottom,hDrawingArea,offsetX,offsetY,rect.right/scale,rect.bottom/scale,SRCCOPY);
		StretchBlt(prevFinalPicture,0,0,rect.right,rect.bottom,finalPicture,0,0,rect.right,rect.bottom,SRCCOPY);
		StretchBlt(finalPicture,0,0,rect.right,rect.bottom,hDrawingArea,0,0,rect.right,rect.bottom,SRCCOPY);
		break;

	case WM_MOUSEMOVE:
		if (wParam & MK_LBUTTON)
		{
			if (tool == ID_PEN||tool==ID_CLEAR)
			{
				
				EndPos = MAKEPOINTS(lParam);
				GetNewCoord(EndPos.x,EndPos.y,offsetX,offsetY,scale);
				Draw(hDrawingArea, tool);
				Draw(hdcMeta,tool);
				StretchBlt(hdc,0, 0, rect.right, rect.bottom,hDrawingArea, offsetX, offsetY,rect.right/scale,rect.bottom/scale, SRCCOPY);
				StartPos = EndPos;
			}
			else
			{
				EndPos = MAKEPOINTS(lParam);
				GetNewCoord(EndPos.x,EndPos.y,offsetX,offsetY,scale);
				StretchBlt(hDrawingArea, 0, 0, rect.right, rect.bottom,finalPicture, 0, 0,rect.right,rect.bottom, SRCCOPY);
				Draw(hDrawingArea,tool);
				StretchBlt(hdc,0, 0, rect.right, rect.bottom,hDrawingArea, offsetX, offsetY,rect.right/scale,rect.bottom/scale, SRCCOPY);

			}
			}

			break;

		case WM_KEYDOWN:
			if (wParam == VK_ESCAPE && !isFirstPoint)
			{
				if (tool==ID_POLYGON)
				{
					isFirstPoint = true;
					numberPoint++;
					POINTSTOPOINT(points[numberPoint], firstPoint);
					MoveToEx(hDrawingArea, EndPos.x, EndPos.y, NULL);
					LineTo(hDrawingArea, firstPoint.x, firstPoint.y);
					free(points);
				}
				else
				{
					isFirstPoint = true;
					free(points);
				}
			StretchBlt(hdc,0,0,rect.right,rect.bottom,hDrawingArea,offsetX,offsetY,rect.right/scale,rect.bottom/scale,SRCCOPY);
			StretchBlt(prevFinalPicture,0,0,rect.right,rect.bottom,finalPicture,0,0,rect.right,rect.bottom,SRCCOPY);
			StretchBlt(finalPicture,0,0,rect.right,rect.bottom,hDrawingArea,0,0,rect.right,rect.bottom,SRCCOPY);
			}
			break;
		case WM_SIZE:
		case WM_SIZING:
			GetClientRect(hMainWnd,&rect);
			FillRect(hdc,&rect,WHITE_BRUSH);
			StretchBlt(hdc,0,0,rect.right,rect.bottom,finalPicture,offsetX,offsetY,rect.right/scale,rect.bottom/scale,SRCCOPY);
			break;

		case WM_CHAR:
			if (tool == ID_TEXT)
			{
				string.append((wchar_t*)(&wParam));
				TextOut(hDrawingArea, StartPos.x, StartPos.y, (LPCSTR)&string[0], string.length());
				TextOut(hdcMeta, StartPos.x, StartPos.y, (LPCSTR)&string[0], string.length());
				StretchBlt(hdc,0, 0, rect.right, rect.bottom,hDrawingArea, 0, 0,rect.right/scale,rect.bottom/scale, SRCCOPY);
				StretchBlt(finalPicture,0, 0, rect.right, rect.bottom,hDrawingArea, 0, 0,rect.right/scale,rect.bottom/scale, SRCCOPY);
			}
			break;

	case WM_MOUSEWHEEL:
			hdc = GetDC(hMainWnd);
			GetClientRect(hMainWnd, &rect);
			if (GET_WHEEL_DELTA_WPARAM(wParam) < 0)
			{
				if (LOWORD(wParam) == MK_CONTROL)
				{
					scale += 0.06;
					StretchBlt(hdc, offsetX, offsetY, rect.right * scale, rect.bottom * scale,finalPicture, 0, 0, rect.right, rect.bottom, SRCCOPY);
				}
				else {
					if (LOWORD(wParam) == MK_SHIFT)
						{
						offsetX += 5;
					StretchBlt(hdc, offsetX, offsetY, rect.right * scale, rect.bottom * scale,finalPicture, 0, 0, rect.right, rect.bottom, SRCCOPY);
						}
					else {
						offsetY += 5;
						StretchBlt(hdc, offsetX, offsetY, rect.right * scale, rect.bottom * scale,finalPicture, 0, 0, rect.right, rect.bottom, SRCCOPY);
										  
					}
				}
			}
			if (GET_WHEEL_DELTA_WPARAM(wParam) > 0)
			{
				if (LOWORD(wParam) == MK_CONTROL)
				{
					scale -= 0.06;
						StretchBlt(hdc, offsetX, offsetY, rect.right * scale, rect.bottom * scale,
						finalPicture, 0, 0, rect.right, rect.bottom, SRCCOPY);
				}
				else {
					if (LOWORD(wParam) == MK_SHIFT)
						{
						offsetX -= 5;
						StretchBlt(hdc, offsetX, offsetY, rect.right * scale, rect.bottom * scale,finalPicture, 0, 0, rect.right, rect.bottom, SRCCOPY);
						}
					else {
						offsetY -= 5;
						StretchBlt(hdc, offsetX, offsetY, rect.right * scale, rect.bottom * scale,finalPicture, 0, 0, rect.right, rect.bottom, SRCCOPY);
										  
					}
				}
			}
			ReleaseDC(hMainWnd, hdc);
			break;
    case WM_DESTROY:
			DeleteObject(hDrawingArea);
			DeleteObject(finalPicture);
			DeleteObject(prevFinalPicture);
			DeleteEnhMetaFile(CloseEnhMetaFile(hdcMeta));
			PostQuitMessage(0);
			break;
	default:	
		return DefWindowProc(hMainWnd, message, wParam, lParam);
    }
	ReleaseDC(hMainWnd, hdc);
    return 0;
}


BOOL CALLBACK DlgProc(HWND hMainWnd, UINT message,
    WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_INITDIALOG:
		bmp=LoadBitmap(hInst,MAKEINTRESOURCE(IDB_BITMAP3));
		hPicture=GetDlgItem(hMainWnd,IDC_STATIC);
		break;
	case WM_COMMAND:
		if (LOWORD(wParam)==IDOK) 
		{
			EndDialog(hMainWnd,0);
			return TRUE;
		} 
		break;
	case WM_PAINT:
		LoadBitmapDLG();
		break;
	case WM_CLOSE:
		EndDialog(hMainWnd,0);
		return TRUE;
	}
	return FALSE;
}


