#include <Windows.h>

HWND hMainWnd,hPicture;
HDC hdcMem, hdcMeta=NULL;
HDC hdc=NULL;
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
DWORD currentColor;
int widthPen, numberPoint,tool= ID_PEN;
HPEN hPen = CreatePen(PS_SOLID, 1, RGB(0, 0, 0));
HBRUSH hBrush=CreateSolidBrush(RGB(255,255,255));			
TCHAR szWindowClass[20];
TCHAR szTemp[] = "D:\\temp.emf";
TCHAR szFile[MAX_PATH], *szFilter, *szFileTitle;
OPENFILENAME openFileName;
PRINTDLG printDlg;
HENHMETAFILE hEnhMetafile, hEnhMetafileCopy;
int offsetX = 0, offsetY = 0;
double scale = 1;
	HDC hDrawingArea;
	HDC finalPicture,prevFinalPicture;



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
			if (isFirstPoint)
			MoveToEx(hdc, StartPos.x, StartPos.y, NULL);
			else MoveToEx(hdc, points[numberPoint].x, points[numberPoint].y, NULL);
			LineTo(hdc, EndPos.x, EndPos.y);
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


HDC InitializeTempDC(HWND hWnd, HDC hdc)
{

	int iWidthMM, iWidthRes, iHeightMM, iHeightRes;

	iWidthMM = GetDeviceCaps(hdc, HORZSIZE);
	iHeightMM = GetDeviceCaps(hdc, VERTSIZE);
	iWidthRes = GetDeviceCaps(hdc, HORZRES);
	iHeightRes = GetDeviceCaps(hdc, VERTRES);

	GetClientRect(hWnd, &rect);

	rect.bottom = (rect.bottom * iHeightMM * 100) / iHeightRes;
	rect.top = (rect.top * iHeightMM * 100) / iHeightRes;
	rect.left = (rect.left * iWidthMM * 100) / iWidthRes;
	rect.right = (rect.right * iWidthMM * 100) / iWidthRes;

	return CreateEnhMetaFile(hdc, (LPSTR)NULL, &rect, (LPSTR)NULL);

}

void LoadBitmapDLG()
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

void InitStructFile(LPSTR title)
{
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
	openFileName.lpstrTitle = title;
}

void SaveEnhFile()
{
	hdc=GetDC(hMainWnd);
	hEnhMetafile = CloseEnhMetaFile(hdcMeta);
	hEnhMetafileCopy = CopyEnhMetaFile(hEnhMetafile, openFileName.lpstrFile);
	DeleteEnhMetaFile(hEnhMetafile);
	hdcMeta=InitializeTempDC(hMainWnd,hdc);
	GetClientRect(hMainWnd, &rect);
	PlayEnhMetaFile(hdcMeta, hEnhMetafileCopy, &rect);
	DeleteEnhMetaFile(hEnhMetafileCopy);
	ReleaseDC(hMainWnd,hdc);
}

void LoadEnhFile()
{
	hdc=GetDC(hMainWnd);
	hEnhMetafile = GetEnhMetaFile(openFileName.lpstrFile);
	GetClientRect(hMainWnd, &rect);
	FillRect(hdc, &rect, hBrush);
	FillRect(hdcMeta, &rect, hBrush);
	PlayEnhMetaFile(hdc,hEnhMetafile,&rect);
	StretchBlt(finalPicture,0,0,rect.right,rect.bottom,hdc,0,0,rect.right,rect.bottom,SRCCOPY);
	StretchBlt(prevFinalPicture,0,0,rect.right,rect.bottom,hdc,0,0,rect.right,rect.bottom,SRCCOPY);
	StretchBlt(hDrawingArea,0,0,rect.right,rect.bottom,hdc,0,0,rect.right,rect.bottom,SRCCOPY);
	DeleteEnhMetaFile(hEnhMetafile);
	ReleaseDC(hMainWnd,hdc);
}

void InitStructPrint()
{
	ZeroMemory(&printDlg, sizeof(printDlg));
	printDlg.lStructSize = sizeof(PRINTDLG);
	printDlg.hwndOwner = hMainWnd;
	printDlg.hDevMode = NULL;
	printDlg.hDevNames = NULL;
	printDlg.Flags = PD_USEDEVMODECOPIESANDCOLLATE | PD_RETURNDC;
	printDlg.nCopies = 1;
	printDlg.nFromPage = 0xFFFF;
	printDlg.nToPage = 0xFFFF;
	printDlg.nMaxPage = 0XFFFF;
	printDlg.nMinPage = 1;
}

void PrintFile()
{
	DOCINFO info;
	HANDLE handle;
	DEVMODE *devMode = (DEVMODE*)GlobalLock(printDlg.hDevMode);

	OpenPrinter((LPSTR)(devMode->dmDeviceName), &handle, NULL);
	ZeroMemory(&info, sizeof(DOCINFO));
	info.cbSize = sizeof(DOCINFO);
	info.lpszDatatype = "emf";
	info.lpszDocName = "NewDoc.emf";
	info.lpszOutput = "NewDoc.pdf";

	StartDoc(printDlg.hDC, &info);
	StartPage(printDlg.hDC);

	hEnhMetafile = CloseEnhMetaFile(hdcMeta);
	GetClientRect(hMainWnd, &rect);
	PlayEnhMetaFile(printDlg.hDC, hEnhMetafile, &rect);

	EndPage(printDlg.hDC);
	EndDoc(printDlg.hDC);

	ClosePrinter(handle);

	DeleteDC(printDlg.hDC);
}

void AddPoint()
{
	if (isFirstPoint)
	{
		numberPoint=0;
		points = (POINT*)calloc(50, sizeof(POINT));
		POINTSTOPOINT(points[numberPoint], EndPos);
		isFirstPoint = false;
		firstPoint = StartPos;
	}
	else
	{
		numberPoint++;
		POINTSTOPOINT(points[numberPoint], EndPos);
	}
}

void ZoomPan(WPARAM wparam,double i,int offset)
{
	if (LOWORD(wparam) == MK_CONTROL)
	{
		scale += i;
		StretchBlt(hdc, 0, 0, rect.right * scale, rect.bottom * scale,
		hdcMem, 0, 0, rect.right, rect.bottom, SRCCOPY);
	}
	else {
		if (LOWORD(wparam) == MK_SHIFT)
			{
			offsetX += offset;
			BitBlt(hdc, offsetX, offsetY, rect.right, rect.bottom, hdcMem, 0, 0, SRCCOPY);
			}
		else {
			offsetY += offset;
			BitBlt(hdc, offsetX, offsetY, rect.right, rect.bottom, hdcMem, 0, 0, SRCCOPY);
										  
		}
	}
}
void InitHDC()
{

		HBITMAP bmFinalCopy;
		HBITMAP bmDrawingCopy;
		HBRUSH brush;
		HDC hdc;
		

		brush = CreateSolidBrush(0xffffff);
		hdc = GetDC(hMainWnd);

		hDrawingArea = CreateCompatibleDC(hdc);
		bmDrawingCopy = CreateCompatibleBitmap(hdc, rect.right, rect.bottom);
		SelectObject(hDrawingArea, bmDrawingCopy);
		SelectObject(hDrawingArea, brush);
		PatBlt(hDrawingArea, 0, 0, rect.right, rect.bottom, PATCOPY);


		finalPicture = CreateCompatibleDC(hdc);
		bmFinalCopy = CreateCompatibleBitmap(hdc, rect.right, rect.bottom);
		SelectObject(finalPicture, bmFinalCopy);
		SelectObject(finalPicture, brush);
		PatBlt(finalPicture, 0, 0, rect.right, rect.bottom, PATCOPY);

		prevFinalPicture = CreateCompatibleDC(hdc);
		bmFinalCopy = CreateCompatibleBitmap(hdc, rect.right, rect.bottom);
		SelectObject(prevFinalPicture, bmFinalCopy);
		SelectObject(prevFinalPicture, brush);
		PatBlt(prevFinalPicture, 0, 0, rect.right, rect.bottom, PATCOPY);

		ReleaseDC(hMainWnd, hdc);
		DeleteObject(brush);
		DeleteObject(bmDrawingCopy);
		DeleteObject(bmFinalCopy);

}

void GetNewCoord(int x, int y, int offsetX, int offsetY, double scale)
{
		x /= scale;
		y /= scale;
		x += offsetX;
		y += offsetY;
}