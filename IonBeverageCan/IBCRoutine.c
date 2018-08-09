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
		135, 110, 300, 500, h_wnd, (HMENU)100, hInstance, NULL); //ID_COMBOBOX=100
	return_ui.h_edit = CreateWindow("combobox", NULL, WS_CHILD | WS_VISIBLE | WS_BORDER |ES_AUTOHSCROLL,
		135, 130, 300,20, h_wnd, (HMENU)101, hInstance, NULL); //ID_EDIT=101
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
	ofn.lpstrFilter = "Every File(*.*)\0*.*\0DLL File\0*.dll\0"; //DLL or The others?
	ofn.lpstrFile = lpstrFile;
	ofn.nMaxFile = 256;

	if (GetOpenFileName(&ofn) != 0) //Set EditBox File Name
	{
		SetDlgItemText(h_wnd, 101, ofn.lpstrFile);
		MessageBox(h_wnd, "File Select","IBC", MB_OK);
	}
}

void SetProcessComboBox(UI ui)
{
	int prior_proc_count = 0; //Why C isn't have CB_CLEAR?
	prior_proc_count = SendMessage(ui.h_combo, CB_GETCOUNT, 0, 0);

	if (prior_proc_count != 0)
	{
		for (int i = 0; i < prior_proc_count; i++)
			SendMessage(ui.h_combo, CB_DELETESTRING, i, 0);
	}

	HWND h_tool = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0); //To use snapshow, Get executing processes in now system

	if ((int)h_tool != -1)
	{
		PROCESSENTRY32 pe32;
		pe32.dwSize = sizeof(PROCESSENTRY32);

		if (Process32First(h_tool, &pe32))
		{
			for (int i = 0; i < 3;i++)
				Process32Next(h_tool, &pe32); //Remove System,Registry String(File Name)
			
			do
			{
				if(strcmp("svchost.exe",pe32.szExeFile)!=0) //Except svchost.exe (Except svchost in First  Version)
					SendMessage(ui.h_combo, CB_ADDSTRING, 0, pe32.szExeFile); //Set ComboBox to Now executing processes 
			} while (Process32Next(h_tool, &pe32));
		}

		CloseHandle(h_tool);
	}
}

