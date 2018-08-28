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
		200, 180, 80, 25, h_wnd, (HMENU)1, hInstance, NULL); //Injection Button;
	CreateWindow("button", "Eject", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
		280, 180, 80, 25, h_wnd, (HMENU)2, hInstance, NULL); //Ejection Button;

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

BOOL InjectDLL(UI ui, HWND h_wnd)
{
	HANDLE h_process = NULL, h_thread = NULL;
	HMODULE h_module = NULL;
	DWORD path_size = 0; DWORD pid;
	LPVOID path_buf = NULL;
	LPTHREAD_START_ROUTINE p_thread_func=NULL;
	char process_name[260];
	char dll_file_path[256];


	if (!GetWindowText(ui.h_combo, process_name, 260))
	{
		MessageBox(h_wnd, "Please Input Process File!", "IBC", MB_OK);
		return FALSE;
	}

	if (!GetWindowText(ui.h_edit, dll_file_path, 256))
	{
		MessageBox(h_wnd, "Please Input DLL File!", "IBC", MB_OK);
		return FALSE;
	}

	pid = GetProcessPID(process_name);
	path_size = (DWORD)(_tcslen(dll_file_path) + 1) * sizeof(TCHAR);

	if (pid == -1)
	{
		MessageBox(h_wnd, "Get PID is Fail!", "IBC", MB_OK);
		return FALSE;
	}

	if (!(h_process = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid))) //Get Process Handle
	{
		MessageBox(h_wnd, "OpenProcess is Fail!", "IBC", MB_OK);
		return FALSE;
	}

	path_buf = VirtualAllocEx(h_process, NULL, path_size, MEM_COMMIT, PAGE_READWRITE); //Memory Allocation(Size:path_size))
	WriteProcessMemory(h_process, path_buf, (LPVOID)dll_file_path, sizeof(dll_file_path), NULL); //Write File Path

	h_module = GetModuleHandle("kernel32.dll");
	p_thread_func = (LPTHREAD_START_ROUTINE)GetProcAddress(h_module, "LoadLibraryW"); //Get LoadLibraryW
	
	h_thread = CreateRemoteThread(h_process, NULL, 0, p_thread_func, path_buf, 0, NULL); //CreateRemoteThread
	
	if (WaitForSingleObject(h_thread, 1000 * 180) == WAIT_TIMEOUT)//Wait Thread Signaled 3 minutes
	{
		CloseHandle(h_thread);
		CloseHandle(h_process);
		return FALSE;
	}

	CloseHandle(h_thread);
	CloseHandle(h_process);
	return TRUE;
}

DWORD GetProcessPID(char* process_name) //Get PID
{
	HWND h_tool = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);


	if ((int)h_tool != -1)
	{
		PROCESSENTRY32 pe32;
		pe32.dwSize = sizeof(PROCESSENTRY32);

		if (Process32First(h_tool, &pe32))
		{
			do
			{
				if (!strcmp(process_name, pe32.szExeFile))
				{
					CloseHandle(h_tool);
					return pe32.th32ProcessID; //success
				}

			} while (Process32Next(h_tool, &pe32));
		}

		CloseHandle(h_tool);
	}
	return -1; //fail
}

BOOL EjectDLL(UI ui, HWND h_wnd)
{
	HANDLE h_process = NULL, h_thread = NULL,h_snapshot=NULL;
	HMODULE h_module = NULL;
	DWORD path_size = 0; DWORD pid;
	LPVOID path_buf = NULL;
	LPTHREAD_START_ROUTINE p_thread_func = NULL;
	char process_name[260];
	char dll_file_path[256];
	char* temp = NULL,*context=NULL;
	MODULEENTRY32 me32 = { sizeof(me32) };
	BOOL is_found_dll=FALSE;



	if (!GetWindowText(ui.h_combo, process_name, 260))
	{
		MessageBox(h_wnd, "Please Input Process File!", "IBC", MB_OK);
		return FALSE;
	}


	if (!GetWindowText(ui.h_edit, dll_file_path, 256))
	{
		MessageBox(h_wnd, "Please Input DLL File!", "IBC", MB_OK);
		return FALSE;
	}

	pid = GetProcessPID(process_name);
	path_size = (DWORD)(_tcslen(dll_file_path) + 1) * sizeof(TCHAR);

	if (pid == -1)
	{
		MessageBox(h_wnd, "Get PID is Fail!", "IBC", MB_OK);
		return FALSE;
	}

	if (!(h_process = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid))) //Get Process Handle
	{
		MessageBox(h_wnd, "OpenProcess is Fail!", "IBC", MB_OK);
		return FALSE;
	}

	if (!SetPrivilege(SE_DEBUG_NAME, TRUE))
	{
		MessageBox(h_wnd, "SetPrivilege is Fail!", "IBC", MB_OK);
		return FALSE;
	}

	h_snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, pid);
	
	if (Module32First(h_snapshot, &me32))
	{
		while (Module32Next(h_snapshot, &me32)) //Search DLL 
		{
			if (!_tcsicmp((LPCSTR)me32.szModule, dll_file_path) ||
				!_tcsicmp((LPCSTR)me32.szExePath, dll_file_path))
			{
				is_found_dll = TRUE;
				break;
			}
		}
	}
	
	if (!is_found_dll)
	{
		CloseHandle(h_snapshot);
		MessageBox(h_wnd, "DLL Search is Fail!", "IBC", MB_OK);
		return FALSE;
	}

	if (!(h_process = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid))) //Get Process Handle
	{
		MessageBox(h_wnd, "OpenProcess is Fail!", "IBC", MB_OK);
		return FALSE;
	}

	h_module = GetModuleHandle("kernel32.dll");
	p_thread_func = (LPTHREAD_START_ROUTINE)GetProcAddress(h_module, "FreeLibrary"); //FreeLibrary
	h_thread = CreateRemoteThread(h_process, NULL, 0, p_thread_func, me32.modBaseAddr, 0, NULL); //CreateRemoteThread
	 
	if (WaitForSingleObject(h_thread, 1000 * 180) == WAIT_TIMEOUT)//Wait Thread Signaled 3 minutes
	{
		CloseHandle(h_thread);
		CloseHandle(h_process);
		CloseHandle(h_snapshot);
		return FALSE;
	}
	CloseHandle(h_thread);
	CloseHandle(h_process);
	CloseHandle(h_snapshot);
	return TRUE;
}

BOOL SetPrivilege(LPCTSTR privilege, BOOL enable_privilege)//This function From in the book(Named ¸®¹ö½Ì ÇÙ½É¿ø¸® author ÀÌ½Â¿ø) 
{ //Get Privilege
	TOKEN_PRIVILEGES tp;
	HANDLE h_token;
	LUID luid;

	if (!OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY,
		&h_token))
		return FALSE;

	if (LookupPrivilegeValue(NULL, privilege, &luid))
		return FALSE;

	tp.PrivilegeCount = 1;
	tp.Privileges[0].Luid = luid;
	if (enable_privilege)
		tp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
	else
		tp.Privileges[0].Attributes = 0;

	if (!AdjustTokenPrivileges(h_token, FALSE, &tp, sizeof(TOKEN_PRIVILEGES),
		(PTOKEN_PRIVILEGES)NULL, (PDWORD)NULL))
		return FALSE;
	if (GetLastError() == ERROR_NOT_ALL_ASSIGNED)
	{
		return FALSE;
	}
	return TRUE;
}
