#pragma comment(linker,"\"/manifestdependency:type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")

#include<windows.h>

TCHAR szClassName[] = TEXT("Window");

LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	static HANDLE hFile;
	static TCHAR szFilePath[MAX_PATH];
	switch (msg)
	{
	case WM_CREATE:
		DragAcceptFiles(hWnd, TRUE);
		break;
	case WM_PAINT:
	{
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hWnd, &ps);
		RECT rect;
		GetClientRect(hWnd, &rect);
		if (hFile)
		{
			TCHAR szText[MAX_PATH];
			wsprintf(szText, TEXT("%s をロックしました。キャンセルするにはESCを押してください。"), szFilePath);
			DrawText(hdc, szText, -1, &rect, DT_CENTER | DT_SINGLELINE | DT_VCENTER);
		}
		else
		{
			DrawText(hdc, TEXT("ロックするファイルをドラッグしてください。"), -1, &rect, DT_CENTER | DT_SINGLELINE | DT_VCENTER);
		}
		EndPaint(hWnd, &ps);
	}
	break;
	case WM_KEYDOWN:
		if (wParam == VK_ESCAPE)
		{
			if (hFile)
			{
				UnlockFile(hFile, 0, 0, 0xffffffff, 0xffffffff);
				CloseHandle(hFile);
				hFile = 0;
				InvalidateRect(hWnd, 0, 1);
			}
		}
		break;
	case WM_DROPFILES:
	{
		const UINT iFileNum = DragQueryFile((HDROP)wParam, -1, NULL, 0);
		TCHAR szTmp[MAX_PATH];
		if (iFileNum == 1)
		{
			if (hFile)
			{
				UnlockFile(hFile, 0, 0, 0xffffffff, 0xffffffff);
				CloseHandle(hFile);
				hFile = 0;
			}
			DragQueryFile((HDROP)wParam, 0, szTmp, MAX_PATH);
			hFile = CreateFile(
				szTmp,
				GENERIC_READ,
				FILE_SHARE_READ | FILE_SHARE_WRITE,
				NULL,
				OPEN_EXISTING,
				FILE_ATTRIBUTE_NORMAL,
				NULL);
			if (hFile != INVALID_HANDLE_VALUE)
			{
				LockFile(hFile, 0, 0, 0xffffffff, 0xffffffff);
				lstrcpy(szFilePath, szTmp);
			}
			else
			{
				hFile = 0;
			}
			InvalidateRect(hWnd, 0, 1);
		}
		DragFinish((HDROP)wParam);
	}
	break;
	case WM_DESTROY:
		if (hFile)
		{
			UnlockFile(hFile, 0, 0, 0xffffffff, 0xffffffff);
			CloseHandle(hFile);
			hFile = 0;
		}
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, msg, wParam, lParam);
	}
	return 0;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPreInst, LPSTR pCmdLine, int nCmdShow)
{
	MSG msg;
	WNDCLASS wndclass = {
		CS_HREDRAW | CS_VREDRAW,
		WndProc,
		0,
		0,
		hInstance,
		0,
		LoadCursor(0,IDC_ARROW),
		(HBRUSH)(COLOR_WINDOW + 1),
		0,
		szClassName
	};
	RegisterClass(&wndclass);
	HWND hWnd = CreateWindow(
		szClassName,
		TEXT("ドロップされたファイルをロックする"),
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT,
		0,
		CW_USEDEFAULT,
		0,
		0,
		0,
		hInstance,
		0
	);
	ShowWindow(hWnd, SW_SHOWDEFAULT);
	UpdateWindow(hWnd);
	while (GetMessage(&msg, 0, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return (int)msg.wParam;
}
