#include <tchar.h>
#include <windows.h>
#include "resource.h"
#include "GameController.h"

LRESULT CALLBACK WindowProc(HWND hWnd,UINT uMsg,WPARAM wParam,LPARAM lParam);

int WINAPI _tWinMain(HINSTANCE hInstance,HINSTANCE hPrevInstance,LPTSTR lpszCmdline,int nCmdShow) {
	LPCTSTR lpszClassName = _T("WTF2048WindowClass");
	LPCTSTR lpszWindowTitle = _T("WTF2048");

	// WNDCLASSEX Initialization
	WNDCLASSEX wcex;
	::ZeroMemory(&wcex,sizeof(WNDCLASSEX));
	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.lpszClassName = lpszClassName;
	wcex.style = CS_DBLCLKS;
	wcex.hInstance = hInstance;
	wcex.lpfnWndProc = WindowProc;
	wcex.hIcon = ::LoadIcon(hInstance,MAKEINTRESOURCE(IDI_MAINICON));
	wcex.hIconSm = ::LoadIcon(hInstance,MAKEINTRESOURCE(IDI_MAINICON_SM));
	wcex.hCursor = ::LoadCursor(NULL,IDC_ARROW);

	// Register window class
	ATOM atom = ::RegisterClassEx(&wcex);
	if (atom == 0) {
		::MessageBox(NULL,_T("RegisterClassEx() failed!"),lpszWindowTitle,MB_ICONERROR);
		return 1;
	}

	// Create application window
	int nWidth = 350;
	int nHeight = 550;
	DWORD dwStyle = WS_OVERLAPPED | WS_SYSMENU | WS_CAPTION | WS_MINIMIZEBOX | WS_VISIBLE;

	RECT rect;
	::GetWindowRect(::GetDesktopWindow(),&rect);
	::InflateRect(&rect,-(rect.right - rect.left - nWidth) / 2,-(rect.bottom - rect.top - nHeight) / 2);
	HWND hWnd = ::CreateWindowEx(0,lpszClassName,lpszWindowTitle,dwStyle,rect.left,rect.top,nWidth,nHeight,HWND_DESKTOP,NULL,hInstance,NULL);
	if (hWnd == NULL) {
		::MessageBox(NULL,_T("CreateWindowEx() failed!"),lpszWindowTitle,MB_ICONERROR);
		return 1;
	}

	// Show application window
	::ShowWindow(hWnd,nCmdShow);

	// Message Pump
	MSG msg;
	BOOL bRet;
	while((bRet = ::GetMessage(&msg,NULL,0,0)) != 0) {
		if (bRet == -1) {
			::MessageBox(NULL,_T("Invalid Window Handle!"),lpszWindowTitle,MB_ICONERROR);
			return -1;
		} else {
			::TranslateMessage(&msg);
			::DispatchMessage(&msg);
		}
	}
	return msg.wParam;
}

GameController gc;

LRESULT CALLBACK WindowProc(HWND hWnd,UINT uMsg,WPARAM wParam,LPARAM lParam) {
	switch (uMsg) {
	case WM_CREATE:
		gc.Initial(hWnd);
		gc.StartGame();
		break;

	case WM_DESTROY:
		gc.Destroy();
		::PostQuitMessage(0);
		break;

	case WM_TIMER:
		gc.NextFrame();
		break;

	case WM_KEYDOWN:
		switch (wParam) {
		case VK_LEFT:
			gc.Swipe(GameController::SWIPE_LEFT);
			break;
		case VK_UP:
			gc.Swipe(GameController::SWIPE_UP);
			break;
		case VK_RIGHT:
			gc.Swipe(GameController::SWIPE_RIGHT);
			break;
		case VK_DOWN:
			gc.Swipe(GameController::SWIPE_DOWN);
			break;
		case VK_ESCAPE:
			if (IDYES == ::MessageBox(hWnd,_T("Are you sure to restart?"),_T("WTF2048"),MB_ICONQUESTION | MB_YESNO)) {
				gc.StartGame();
			}
			break;
		}
		if (gc.IsGameOver()) {
			if (IDYES == ::MessageBox(hWnd,_T("Game over, try again?"),_T("WTF2048"),MB_ICONWARNING | MB_YESNO)) {
				gc.StartGame();
			}
		}
		break;

	case WM_PAINT:
		{
			PAINTSTRUCT ps;
			::BeginPaint(hWnd,&ps);
			gc.OutputUI(ps.hdc);
			::EndPaint(hWnd,&ps);
		}
		break;

	default:
		return ::DefWindowProc(hWnd,uMsg,wParam,lParam);
	}
	return 0;
}
