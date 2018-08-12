#include <Windows.h> 
#include <stdlib.h>
#include <string.h>
#include <tchar.h>
#include <windows.h>
#include "IBCRoutine.h"

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

HINSTANCE g_hInst;
UI ui;

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance
	, LPSTR lpszCmdParam, int nCmdShow)
{
	HWND h_wnd;
	MSG message;
	WNDCLASS wnd_class;
	
	wnd_class.lpfnWndProc = (WNDPROC)WndProc;
	g_hInst = hInstance;
	
	SetWndClass(&wnd_class, hInstance);
	RegisterClass(&wnd_class);

	h_wnd = CreateWindow(CLS_NAME, CLS_NAME, WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, 0, 600, 400,
		NULL, (HMENU)NULL, hInstance, NULL);

	ShowWindow(h_wnd, nCmdShow);
	ProcMessage(&message);

	return message.wParam;
}

LRESULT CALLBACK WndProc(HWND h_wnd, UINT message, WPARAM w_param, LPARAM l_param)
{
	HDC hdc;
	PAINTSTRUCT ps;
	switch (message)
	{
	case WM_CREATE:
		ui = CreateUI(g_hInst, h_wnd);
		return 0;
	case WM_PAINT:
		DrawFormerText(h_wnd, &hdc, &ps, ui.font);
		return 0;
	case WM_COMMAND:
		switch (LOWORD(w_param))
		{
		case 0: //DLL File Select Open
			SelectDLLFIle(ui, h_wnd);
			return 0;
		case 1:
			if (InjectDLL(ui,h_wnd))
				MessageBox(h_wnd, "Injection success!", "IBC", MB_OK);
			else 
				MessageBox(h_wnd, "Injection fail!", "IBC", MB_OK);
			return 0;
		case 100://ComboBox Clicked
			switch (HIWORD(w_param))
			{
			case CBN_DROPDOWN:
				SetProcessComboBox(ui);
				break;
			}
			return 0;
		}
		return 0;
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	}

	return(DefWindowProc(h_wnd, message, w_param, l_param));
}