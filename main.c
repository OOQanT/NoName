#include <windows.h>
#include "resource.h"
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

HINSTANCE g_hInst;
HWND hWndMain;
LPCTSTR lpszClass = TEXT("ReadDIB");

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance
	, LPSTR lpszCmdParam, int nCmdShow)
{
	HWND hWnd;
	MSG Message;
	WNDCLASS WndClass;
	g_hInst = hInstance;

	WndClass.cbClsExtra = 0;
	WndClass.cbWndExtra = 0;
	WndClass.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	WndClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	WndClass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	WndClass.hInstance = hInstance;
	WndClass.lpfnWndProc = WndProc;
	WndClass.lpszClassName = lpszClass;
	WndClass.lpszMenuName = MAKEINTRESOURCE(IDR_MENU1);
	WndClass.style = CS_HREDRAW | CS_VREDRAW;
	RegisterClass(&WndClass);

	hWnd = CreateWindow(lpszClass, lpszClass, WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
		NULL, (HMENU)NULL, hInstance, NULL);
	ShowWindow(hWnd, nCmdShow);

	while (GetMessage(&Message, NULL, 0, 0)) {
		TranslateMessage(&Message);
		DispatchMessage(&Message);
	}
	return (int)Message.wParam;
}

//...\Study-Lang\WINAPI\ApiPrj2003\[ skk ]\ch29-비트맵 추가\ReadDIB-반전
//...\Study-Lang\WINAPI\ApiPrj2003\[ skk ]\ch29-비트맵 추가\ReadDIB-GrayLevel

BITMAPFILEHEADER *fh = NULL;
BITMAPINFOHEADER *ih;
int bx, by;
BYTE *pRaster;
HANDLE hFile;
static int Op = 0;
static int count = FALSE;

void LoadDIB(LPCTSTR Path)
{
	HANDLE hFile;
	DWORD FileSize, dwRead;

	hFile = CreateFile(Path, GENERIC_READ, 0, NULL,
		OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile == INVALID_HANDLE_VALUE) {
		return;
	}

	if (fh) {
		free(fh);
		fh = NULL;
	}
	FileSize = GetFileSize(hFile, NULL);
	fh = (BITMAPFILEHEADER *)malloc(FileSize);
	ReadFile(hFile, fh, FileSize, &dwRead, NULL);
	CloseHandle(hFile);

	pRaster = (PBYTE)fh + fh->bfOffBits;
	ih = (BITMAPINFOHEADER *)((PBYTE)fh + sizeof(BITMAPFILEHEADER));
	bx = ih->biWidth;
	by = ih->biHeight;
}

BOOL CALLBACK DlgProc(HWND hDlg, UINT iMessage, WPARAM wParam, LPARAM lParam){
	static int color;
	int TempPos;
	switch (iMessage) {
	case WM_INITDIALOG:
		color = GetDlgItem(hDlg, IDC_SCROLLBAR1);
		/*SetScrollRange(color, SB_CTL, 0, 100, TRUE);
		SetScrollPos(color, SB_CTL, 50, TRUE);*/

		return TRUE;
	case WM_HSCROLL:
		TempPos = GetScrollPos((HWND)lParam, SB_CTL);
		switch (LOWORD(wParam)) {
		case SB_LINELEFT:
			TempPos = max(0, TempPos - 1);
			break;
		case SB_LINERIGHT:
			TempPos = min(100, TempPos + 1);
			break;
		case SB_PAGELEFT:
			TempPos = max(0, TempPos - 10);
			break;
		case SB_PAGERIGHT:
			TempPos = min(100, TempPos + 10);
			break;
		case SB_THUMBTRACK:
			TempPos = HIWORD(wParam);
			break;
		}

		SetScrollPos((HWND)lParam, SB_CTL, TempPos, TRUE);

		if ((HWND)lParam == color)
		{
			if (TempPos >= 50){
				color = TempPos - 50;
			}
			else{
				color = 50 - TempPos;
			}

			SetDlgItemInt(hDlg, IDC_State, color, FALSE);
		}


		//InvalidateRect(hWnd, NULL, FALSE);
		/*PaintTheBlock(hCtrlBlock, RGB(iRed, iGreen, iBlue),
		iWidth);*/
		return TRUE;
	case WM_COMMAND:
		switch (LOWORD(wParam)) {
		case IDOK:
			//iCurrentPenWidth =
			//	(SendMessage(hCombo,CB_GETCURSEL, 0, 0) + 1) * 3;
			/*iCurrentPenWidth = GetDlgItemInt(hDlg, IDC_COMBO1, NULL, FALSE);
			iCurrentPenColor = RGB(iRed, iGreen, iBlue);*/
			EndDialog(hDlg, IDOK);
			return TRUE;
		case IDCANCEL:
			EndDialog(hDlg, IDCANCEL);
			return TRUE;
		}
	case WM_PAINT:
		//PaintTheBlock(hCtrlBlock, RGB(iRed, iGreen, iBlue), iWidth);
		break;
	}
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM lParam)
{
	HDC hdc;
	PAINTSTRUCT ps;
	OPENFILENAME OFN;
	TCHAR lpstrFile[MAX_PATH] = ""; //배열의 이름은 그 배열의 시작 주소이다.
	BYTE *pBitmap;
	int cx, cy, i, j;
	
	switch (iMessage) {
	case WM_COMMAND:
		switch (LOWORD(wParam)) {
			case IDR_OPEN:
				memset(&OFN, 0, sizeof(OPENFILENAME));
				OFN.lStructSize = sizeof(OPENFILENAME);
				OFN.hwndOwner = hWnd;
				OFN.lpstrFilter = "Bitmap File(*.bmp)\0*.bmp\0";
				OFN.lpstrFile = lpstrFile; //포인터를 안써도 바로 사용 가능
				OFN.nMaxFile = MAX_PATH;
				if (GetOpenFileName(&OFN) != 0) {
					LoadDIB(lpstrFile);
					InvalidateRect(hWnd, NULL, TRUE);
					count = TRUE;
				}
				break;
			case IDR_DIALOG:
				if (count)
					DialogBox(g_hInst, MAKEINTRESOURCE(IDD_DIALOG1), hWnd, DlgProc);
				break;

		}
		return 0;
	case WM_RBUTTONDOWN:
		if (hFile == INVALID_HANDLE_VALUE) {
			return 0;
		}
		pBitmap = (BYTE *)pRaster;
		cx = bx; cy = by;

		for (i = 0; i < cx * cy * 3; i++){ // 데이터가 한줄로 들어가있기 때문에 이렇게 해도 밑과 똑같은 효과를 가져옴
			*(pBitmap + i) = ~*(pBitmap + i); // ~ not 연산자
		}

		InvalidateRect(hWnd, NULL, TRUE);
		return 0;
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		if (fh) {
			SetDIBitsToDevice(hdc, 0, 0, bx, by, 0, 0, 0, by,
				pRaster, (BITMAPINFO *)ih, DIB_RGB_COLORS);
		}
		EndPaint(hWnd, &ps);
		return 0;

	case WM_DESTROY:
		if (fh)
			free(fh);
		PostQuitMessage(0);
		return 0;
	}
	return(DefWindowProc(hWnd, iMessage, wParam, lParam));
}