#include "IBCRoutine.h"

void ProcMessage(MSG* msg)
{
	while (GetMessage(msg, 0, 0, 0)) 
	{
		TranslateMessage(msg);
		DispatchMessage(msg);
	}
}
UI CreateUI(HINSTANCE hInstance,HWND h_wnd) //Making UI
{
	UI return_ui;

	return_ui.h_combo = CreateWindow("combobox", NULL, WS_CHILD | WS_VISIBLE | CBS_DROPDOWN,
		135, 110, 300, 80, h_wnd, (HMENU)100, hInstance, NULL); //ID_COMBOBOX=100
	return_ui.h_edit = CreateWindow("combobox", NULL, WS_CHILD | WS_VISIBLE | WS_BORDER |ES_AUTOHSCROLL,
		135, 130, 300, 25, h_wnd, (HMENU)101, hInstance, NULL); //ID_EDIT=101
	return_ui.font = CreateFont(15, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
		ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
		DEFAULT_QUALITY, VARIABLE_PITCH | FF_SWISS,
		"Sans");
	CreateWindow("button", "...", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
		435,130, 50, 25, h_wnd, (HMENU)0, hInstance, NULL); //If it is clicked, open file selecter  
	CreateWindow("button", "Inject", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
		250, 180, 80, 25, h_wnd, (HMENU)1, hInstance, NULL); //Injection Button;

	return return_ui;
}

void DrawFormerText(HWND h_wnd,HDC* hdc, PAINTSTRUCT* ps, HFONT font)
{
	*hdc = BeginPaint(h_wnd, ps);
	char str1[] = "Process";//Former Combo Text
	char str2[] = "DLL File"; //Former Edit Text
	SelectObject(*hdc, font);
	TextOut(*hdc, 80, 110, str1, strlen(str1)); 
	TextOut(*hdc, 80, 130, str2, strlen(str2));
	EndPaint(h_wnd, ps);
}

void SetWndClass(WNDCLASS* wnd_class, HINSTANCE hInstance)
{
	wnd_class->cbClsExtra = 0;
	wnd_class->cbWndExtra = 0;
	wnd_class->hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	wnd_class->hCursor = LoadCursor(NULL, IDC_ARROW);
	wnd_class->hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wnd_class->hInstance = hInstance;
	wnd_class->lpszClassName = CLS_NAME;
	wnd_class->lpszMenuName = NULL;
	wnd_class->style = CS_HREDRAW | CS_VREDRAW;
}

void SelectDLLFIle(UI ui,HWND h_wnd)
{
	OPENFILENAME ofn;
	char lpstrFile[MAX_PATH] = "";

	memset(&ofn, 0, sizeof(OPENFILENAME));
	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.hwndOwner = h_wnd;
	ofn.lpstrFilter = "Every File(*.*)\0*.*\0DLL File\0*.dll\0";
	ofn.lpstrFile = lpstrFile;
	ofn.nMaxFile = 256;

	if (GetOpenFileName(&ofn) != 0)
	{
		SetDlgItemText(h_wnd, 101, ofn.lpstrFile);
		MessageBox(h_wnd, "File Select","IBC", MB_OK);
	}
}