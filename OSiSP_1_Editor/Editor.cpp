#include <Windows.h>
#include <string>
#include "string.h"
#include "resource.h"
#include "afxres.h"
#include "Menu.h"



HWND hMainWnd,hPicture;
HDC hdc,hdcMem, hdcMeta=NULL;
HBITMAP hBitmap,bmp,hOldBitmap;
HGDIOBJ oldBitmap;
HINSTANCE hInst;
bool flag,isCancel,isFirstPoint=true;
POINTS StartPos,EndPos, firstPoint;
RECT rect;
POINT *points;
std::wstring string;
PAINTSTRUCT paintstr;
CHOOSECOLOR colorDlg;
COLORREF colors[16];
DWORD currentColorPen,currentColorBrush,currentColor;
int tool= ID_PEN;
HPEN hPen = CreatePen(PS_SOLID, 1, RGB(0, 0, 0));
HBRUSH hBrush=CreateSolidBrush(RGB(255,255,255));
int widthPen, numberPoint;			
TCHAR szWindowClass[50];
TCHAR szTemp[] = "D:\\temp.emf";
TCHAR szFile[MAX_PATH], *szFilter, *szFileTitle, *szTitle="Save";
OPENFILENAME openFileName;
HENHMETAFILE hEnhMetafile, hEnhMetafileCopy;

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
    WndClsEx.hbrBackground = (HBRUSH)(LTGRAY_BRUSH);
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
void BitmapCreate(HDC hdc, RECT rect)
{
   
    HDC hdcMem = CreateCompatibleDC(hdc);
	hOldBitmap=hBitmap;
    hBitmap = CreateCompatibleBitmap(hdc, rect.right, rect.bottom);
    HANDLE oldBitmap = SelectObject(hdcMem, hBitmap);
    BitBlt(hdcMem, 0, 0, rect.right, rect.bottom, hdc, 0, 0, SRCCOPY);
    SelectObject(hdcMem, oldBitmap);
    DeleteObject(oldBitmap);
    DeleteDC(hdcMem);

}
void BitmapLoad(HDC hdc, HWND hWnd, RECT rect)
{
	HGDIOBJ oldBitmap;
	BITMAP bitmap;
	HDC hdcMem = CreateCompatibleDC(hdc);
	if (isCancel)
	{	oldBitmap = SelectObject(hdcMem, hOldBitmap);
		GetObject(hOldBitmap, sizeof(bitmap), &bitmap);
		hBitmap=hOldBitmap;
	}
	else
	{
		oldBitmap = SelectObject(hdcMem, hBitmap);
		GetObject(hBitmap, sizeof(bitmap), &bitmap);
	}
		BitBlt(hdc, 0, 0, rect.right, rect.bottom, hdcMem, 0, 0, SRCCOPY);
		SelectObject(hdcMem, oldBitmap);
    DeleteDC(hdcMem);
}

void Draw(HDC hdc, int tool)
{
	SelectObject(hdc,hPen);
	SelectObject(hdc,hBrush);
	switch (tool)
	{
		case ID_PEN:
		case ID_CLEAR:
		case ID_LINE:
			MoveToEx(hdc, StartPos.x, StartPos.y, NULL);
			LineTo(hdc, EndPos.x, EndPos.y);
			break;
		case ID_BROKEN:	
		case ID_POLYGON:
			Polyline(hdc, points, numberPoint + 1);
			break;
		case ID_RECTANGLE:
			Rectangle(hdc, StartPos.x, StartPos.y, EndPos.x, EndPos.y);
			break;
		case ID_ELLIPSE:
			Ellipse(hdc, StartPos.x, StartPos.y, EndPos.x, EndPos.y);
			break;
	}
}

void ChangeColor(int command)
{
	ZeroMemory(&colorDlg, sizeof(colorDlg));
	colorDlg.lStructSize = sizeof(CHOOSECOLOR);
	colorDlg.hwndOwner = hMainWnd;
	colorDlg.lpCustColors = (LPDWORD)colors;
	colorDlg.rgbResult = currentColor;
	colorDlg.Flags = CC_RGBINIT;

	if (ChooseColor(&colorDlg))
	{
		if (command==ID_COLOR)
		{
			DeleteObject(hPen);
			currentColor = colorDlg.rgbResult;
			hPen = CreatePen(PS_SOLID, widthPen, currentColor);
		}
		else
		{
			DeleteObject(hBrush);
			hBrush=CreateSolidBrush(colorDlg.rgbResult);
		}
	}

}

LRESULT CALLBACK WndProc(HWND hMainWnd, UINT message,
    WPARAM wParam, LPARAM lParam)
{
    switch (message) 
    {
	case WM_CREATE:
		break;
	case WM_COMMAND:
        switch (LOWORD(wParam)) {
		case ID_SAVE:
			GetClientRect(hMainWnd, &rect);
			szFile[0] = '\0';
			szFilter = "Metafile (*.emf)\0*.emf\0Все файлы (*.*)\0*.*\0";
			ZeroMemory(&openFileName, sizeof(openFileName));
			openFileName.lStructSize = sizeof(OPENFILENAME);
			openFileName.hwndOwner = hMainWnd;
			openFileName.lpstrFilter = szFilter;
			openFileName.lpstrFile = szFile;
			openFileName.nMaxFile = sizeof(szFile);
			openFileName.lpstrFileTitle = (LPSTR)NULL;
			openFileName.lpstrInitialDir = (LPSTR)NULL;
			openFileName.Flags = OFN_PATHMUSTEXIST | OFN_OVERWRITEPROMPT;
			openFileName.lpstrTitle = "Save as";

			if (GetSaveFileName(&openFileName) == TRUE)
			{
								
				HDC hdc = GetDC(hMainWnd);
				HDC metafile;
				GetClientRect(hMainWnd, &rect);
			//	HBITMAP hBitmap = CreateCompatibleBitmap(hdc, rect.right, rect.bottom);
				SelectObject(hdc, hBitmap);
				metafile = CreateEnhMetaFile(hdc, (LPCSTR)openFileName.lpstrFile, &rect, NULL);

				BitBlt(metafile, 0, 0, rect.right, rect.bottom, hdc, 0, 0, SRCCOPY);
				CloseEnhMetaFile(metafile);
			}
			break;
		case ID_OPEN:
			szFile[0] = '\0';
			szFilter = ".EMF\0\0";
			ZeroMemory(&openFileName, sizeof(openFileName));
			openFileName.lStructSize = sizeof(OPENFILENAME);
			openFileName.hwndOwner = hMainWnd;
			openFileName.lpstrFilter = szFilter;
			openFileName.lpstrFile = szFile;
			openFileName.nMaxFile = sizeof(szFile);	
			openFileName.lpstrFileTitle = (LPSTR)NULL;
			openFileName.lpstrInitialDir = (LPSTR)NULL;
			openFileName.lpstrTitle = "Open";
			openFileName.nFilterIndex = 1;
			openFileName.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

			if (GetOpenFileName(&openFileName) == TRUE)
			{

				HDC hdc=GetDC(hMainWnd);
				hEnhMetafile = GetEnhMetaFile(openFileName.lpstrFile);
				GetClientRect(hMainWnd, &rect);
				SelectObject(hdc,hBrush);
				SelectObject(hdc,hPen);
				FillRect(hdc, &rect,hBrush);

				PlayEnhMetaFile(hdc, hEnhMetafile, &rect);
				DeleteEnhMetaFile(hEnhMetafile);

				hEnhMetafile = GetEnhMetaFile(openFileName.lpstrFile);
				GetClientRect(hMainWnd, &rect);
				PlayEnhMetaFile(hdcMeta, hEnhMetafile, &rect);
				DeleteEnhMetaFile(hEnhMetafile);
			}
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
		hdc = GetDC(hMainWnd);
		switch (tool)
		{
			case ID_ELLIPSE:
			case ID_RECTANGLE:
			case ID_LINE:
				Draw(hdc, tool);
				flag = false;
				break;
			case ID_TEXT:
				StartPos = EndPos;
				string.clear();
				break;
			case ID_POLYGON:
			case ID_BROKEN:
				if (isFirstPoint)
				{
					numberPoint=0;
					points = (POINT*)calloc(50, sizeof(POINT));
					POINTSTOPOINT(points[numberPoint], EndPos);
					isFirstPoint = false;
					firstPoint = EndPos;
				}
				else
				{
					numberPoint++;
					POINTSTOPOINT(points[numberPoint], EndPos);
				}
				Draw(hdc, tool);
				break;
			}
		BitmapCreate(hdc,rect);
		break;


	case WM_MOUSEMOVE:
		hdc = GetDC(hMainWnd);
		if (wParam & MK_LBUTTON)
		{
			if (tool == ID_PEN||tool==ID_CLEAR)
			{
				EndPos = MAKEPOINTS(lParam);
				Draw(hdc, tool);
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
			if ((GetAsyncKeyState(VK_CONTROL)) && (GetAsyncKeyState(0x5A))){
				InvalidateRect(hMainWnd, NULL, TRUE);
				UpdateWindow(hMainWnd);
			}
			if (wParam == VK_ESCAPE && !isFirstPoint&&tool==ID_POLYGON)
		{
			isFirstPoint = true;
			numberPoint++;
			POINTSTOPOINT(points[numberPoint], firstPoint);
			Draw(hdc, tool);
			free(points);
		}
			if (wParam == VK_ESCAPE && !isFirstPoint&&tool==ID_BROKEN)
		{
			isFirstPoint = true;
			Draw(hdc, tool);
			free(points);
		}
			break;

		case WM_CHAR:

		if (tool == ID_TEXT)
		{
			string.append((wchar_t*)(&wParam));
			TextOut(hdc, StartPos.x, StartPos.y, (LPCSTR)&string[0], string.length());
			BitmapCreate(hdc,rect);
		}
		break;

	case WM_PAINT:
		hdc = BeginPaint(hMainWnd, &paintstr);
			BitmapLoad(hdc, hMainWnd, rect);
			GetClientRect(hMainWnd, &rect);
        EndPaint(hMainWnd, &paintstr);
		break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hMainWnd, message, wParam, lParam);
    }
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
		{
			BITMAP bitmap;
			PAINTSTRUCT paintstr;

			HDC hdc = BeginPaint(hPicture,&paintstr);
			HDC hdcMem = CreateCompatibleDC(hdc);
			HGDIOBJ hBmOld = SelectObject(hdcMem,bmp);
			GetObject(bmp,sizeof(bitmap),&bitmap);
			BitBlt(hdc,0,0,bitmap.bmWidth,bitmap.bmHeight,hdcMem,0,0,SRCCOPY);
			SelectObject(hdcMem,hBmOld);
			DeleteDC(hdcMem);
			EndPaint(hPicture,&paintstr);
		}
		break;
	case WM_CLOSE:
		EndDialog(hMainWnd,0);
		return TRUE;
	}
	return FALSE;
}


