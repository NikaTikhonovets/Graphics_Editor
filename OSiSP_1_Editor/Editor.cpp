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

	RegisterClass(hInstance);
    InitInstance(hInstance,nCmdShow);
	hInst=hInstance;
	CreateMyMenu(hMainWnd,hInst);
	while (GetMessage(&Msg, NULL, 0, 0)) 
    {
        TranslateMessage(&Msg);
        DispatchMessage(&Msg);
    }

    return (int)Msg.wParam;
}




LRESULT CALLBACK WndProc(HWND hMainWnd, UINT message,
    WPARAM wParam, LPARAM lParam)
{
	hdc = GetDC(hMainWnd);


    switch (message) 
    {
	case WM_CREATE:
		hdcMeta=InitializeTempDC(hMainWnd,hdc);
		break;
	case WM_COMMAND:
        switch (LOWORD(wParam)) {
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
				isCancel=true;
				InvalidateRect(hMainWnd, NULL, TRUE);
				UpdateWindow(hMainWnd);
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
				FillRect(hdc,&rect,WHITE_BRUSH);
				FillRect(hdcMeta,&rect,WHITE_BRUSH);
				break;
			default:
				tool = LOWORD(wParam);
				DeleteObject(hPen);
				hPen = CreatePen(PS_SOLID, widthPen, currentColor);
			}
			break;
	case WM_LBUTTONDOWN:
		StartPos=MAKEPOINTS(lParam);
		flag=false;
		ReleaseDC(hMainWnd, hdc);
		break;
	case WM_LBUTTONUP:
		EndPos=MAKEPOINTS(lParam);
		switch (tool)
		{
			case ID_ELLIPSE:
			case ID_RECTANGLE:
			case ID_LINE:
				Draw(hdc, tool);
				Draw(hdcMeta,tool);
				flag = false;
				break;
			case ID_TEXT:
				StartPos = EndPos;
				string.clear();
				break;
			case ID_POLYGON:
			case ID_BROKEN:
				AddPoint();
				Draw(hdc, tool);
				Draw(hdcMeta,tool);
				break;
		}
		BitmapCreate(hdc,rect);
		break;

	case WM_MOUSEMOVE:
		if (wParam & MK_LBUTTON)
		{
			if (tool == ID_PEN||tool==ID_CLEAR)
			{
				EndPos = MAKEPOINTS(lParam);
				Draw(hdc, tool);
				Draw(hdcMeta,tool);
				StartPos = EndPos;
			}
			else
			{
				SetROP2(hdc, R2_NOTXORPEN);
				if (flag)
					Draw(hdc,tool);
				EndPos = MAKEPOINTS(lParam);
				Draw(hdc,tool);
				flag = true;
			}
			}
			GetFocus();
			ReleaseDC(hMainWnd, hdc);
			break;

		case WM_KEYDOWN:
			if ((GetAsyncKeyState(VK_CONTROL)) && (GetAsyncKeyState(0x5A)))
			{
				InvalidateRect(hMainWnd, NULL, TRUE);
				UpdateWindow(hMainWnd);
			}
			if (wParam == VK_ESCAPE && !isFirstPoint&&tool==ID_POLYGON)
			{
				if (tool==ID_POLYGON)
				{
					isFirstPoint = true;
					numberPoint++;
					POINTSTOPOINT(points[numberPoint], firstPoint);
					Draw(hdc, tool);
					Draw(hdcMeta,tool);
					free(points);
				}
				else
				{
					isFirstPoint = true;
					Draw(hdc, tool);
					Draw(hdcMeta,tool);
					free(points);
				}
			}
			break;

		case WM_CHAR:
			if (tool == ID_TEXT)
			{
				string.append((wchar_t*)(&wParam));
				TextOut(hdc, StartPos.x, StartPos.y, (LPCSTR)&string[0], string.length());
				TextOut(hdcMeta, StartPos.x, StartPos.y, (LPCSTR)&string[0], string.length());
				BitmapCreate(hdc,rect);
			}
			break;

	case WM_MOUSEWHEEL:
			hdc = GetDC(hMainWnd);
			hdcMem = CreateCompatibleDC(hdc);
			oldBitmap = SelectObject(hdcMem, hBitmap);
			GetClientRect(hMainWnd, &rect);
			BitmapLoad(hdcMem, hMainWnd, rect);
			if (GET_WHEEL_DELTA_WPARAM(wParam) < 0)
				ZoomPan(wParam,0.04,5);
			else
				ZoomPan(wParam,-0.04,-5);
			BitmapCreate(hdc,rect);
			ReleaseDC(hMainWnd, hdc);
			break;
	case WM_PAINT:
			hdc = BeginPaint(hMainWnd, &paintstr);
				BitmapLoad(hdc, hMainWnd, rect);
			EndPaint(hMainWnd, &paintstr);
			break;
    case WM_DESTROY:
			DeleteEnhMetaFile(CloseEnhMetaFile(hdcMeta));
			PostQuitMessage(0);
			break;

	default:
		ReleaseDC(hMainWnd, hdc);
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


