// Sketch.cpp : アプリケーションのエントリ ポイントを定義します。
//

#include "stdafx.h"
#include "Sketch.h"

#define MAX_LOADSTRING 100

// グローバル変数:
HINSTANCE hInst;                                // 現在のインターフェイス
WCHAR szTitle[MAX_LOADSTRING];                  // タイトル バーのテキスト
WCHAR szWindowClass[MAX_LOADSTRING];            // メイン ウィンドウ クラス名

// このコード モジュールに含まれる関数の宣言を転送します:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: ここにコードを挿入してください。

    // グローバル文字列を初期化する
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_SKETCH, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // アプリケーション初期化の実行:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_SKETCH));

    MSG msg;

    // メイン メッセージ ループ:
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return (int) msg.wParam;
}

void GetLargestDisplayMode(int * pcxBitmap, int * pcyBitmap)
{
	DEVMODE devmode;
	int iModeNum = 0;
	*pcxBitmap = *pcyBitmap = 0;
	ZeroMemory(&devmode, sizeof(DEVMODE));
	devmode.dmSize = sizeof(DEVMODE);

	while (EnumDisplaySettings(NULL, iModeNum++, &devmode))
	{
		*pcxBitmap = max(*pcxBitmap, (int)devmode.dmPelsWidth);
		*pcyBitmap = max(*pcyBitmap, (int)devmode.dmPelsHeight);
	}
}

//
//  関数: MyRegisterClass()
//
//  目的: ウィンドウ クラスを登録します。
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_SKETCH));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_SKETCH);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

//
//   関数: InitInstance(HINSTANCE, int)
//
//   目的: インスタンス ハンドルを保存して、メイン ウィンドウを作成します
//
//   コメント:
//
//        この関数で、グローバル変数でインスタンス ハンドルを保存し、
//        メイン プログラム ウィンドウを作成および表示します。
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // グローバル変数にインスタンス ハンドルを格納する

   HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

//
//  関数: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  目的: メイン ウィンドウのメッセージを処理します。
//
//  WM_COMMAND  - アプリケーション メニューの処理
//  WM_PAINT    - メイン ウィンドウを描画する
//  WM_DESTROY  - 中止メッセージを表示して戻る
//
//
LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	static BOOL fLeftButtonDown, fRightButtonDown;
	static HBITMAP hBitmap;
	static HDC hdcMem;
	static int cxBitmap, cyBitmap, cxClient, cyClient, xMouse, yMouse;
	HDC hdc;
	PAINTSTRUCT ps;

	switch (message)
	{
	case WM_CREATE:
		GetLargestDisplayMode(&cxBitmap, &cyBitmap);
		hdc = GetDC(hwnd);
		hBitmap = CreateCompatibleBitmap(hdc, cxBitmap,
			cyBitmap);
		hdcMem = CreateCompatibleDC(hdc);
		ReleaseDC(hwnd, hdc);
		if (!hBitmap) // no memory for bitmap
		{
			DeleteDC(hdcMem);
			return -1;
		}
		SelectObject(hdcMem, hBitmap);
		PatBlt(hdcMem, 0, 0, cxBitmap, cyBitmap, WHITENESS);
		return 0;
	case WM_SIZE:
		cxClient = LOWORD(lParam);
		cyClient = HIWORD(lParam);
		return 0;
	case WM_LBUTTONDOWN:
		if (!fRightButtonDown)
			SetCapture(hwnd);
		xMouse = LOWORD(lParam);
		yMouse = HIWORD(lParam);
		fLeftButtonDown = TRUE;
		return 0;
	case WM_LBUTTONUP:
		if (fLeftButtonDown)
			SetCapture(NULL);
		fLeftButtonDown = FALSE;
		return 0;

	case WM_RBUTTONDOWN:
		if (!fLeftButtonDown)
			SetCapture(hwnd);

		xMouse = LOWORD(lParam);
		yMouse = HIWORD(lParam);
		fRightButtonDown = TRUE;
		return 0;

	case WM_RBUTTONUP:
		if (fRightButtonDown)
			SetCapture(NULL);

		fRightButtonDown = FALSE;
		return 0;
	case WM_MOUSEMOVE:
		if (!fLeftButtonDown && !fRightButtonDown)
			return 0;
		hdc = GetDC(hwnd);
		SelectObject(hdc, GetStockObject(fLeftButtonDown ? BLACK_PEN : WHITE_PEN));
		SelectObject(hdcMem, GetStockObject(fLeftButtonDown ? BLACK_PEN : WHITE_PEN));
		MoveToEx(hdc, xMouse, yMouse, NULL);
		MoveToEx(hdcMem, xMouse, yMouse, NULL);
		xMouse = (short)LOWORD(lParam);
		yMouse = (short)HIWORD(lParam);
		LineTo(hdc, xMouse, yMouse);
		LineTo(hdcMem, xMouse, yMouse);
		ReleaseDC(hwnd, hdc);
		return 0;
	case WM_PAINT:
		hdc = BeginPaint(hwnd, &ps);

		BitBlt(hdc, 0, 0, cxClient, cyClient, hdcMem, 0, 0, SRCCOPY);
		EndPaint(hwnd, &ps);
		return 0;
	case WM_DESTROY:
		DeleteDC(hdcMem);
		DeleteObject(hBitmap);
		PostQuitMessage(0);
		return 0;
	}
	return DefWindowProc(hwnd, message, wParam, lParam);
}

// バージョン情報ボックスのメッセージ ハンドラーです。
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}
