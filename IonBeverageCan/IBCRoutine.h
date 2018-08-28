#pragma once //Routine Header
#ifndef _IBC_ROUTINE_H__
#define _IBC_ROUTINE_H__
#define ID_COMBOBOX 100;
#define ID_EDIT 101;
#define CLS_NAME "IBC"
#include <Windows.h>
#include <Tlhelp32.h>
#include <tchar.h>
#include <stdio.h>
#include <sys/types.h>
typedef struct ui
{
	HWND h_combo; //Process Select ComboBox
	HWND h_edit; //File Select Edit
	HFONT font; // Font
	//HWND inject_button; Inject Button
}UI;

void ProcMessage(MSG*);
UI CreateUI(HINSTANCE,HWND);
void DrawFormerText(HWND,HDC*, PAINTSTRUCT*, HFONT);
void SetWndClass(WNDCLASS*, HINSTANCE);
void ButtonProc(WPARAM, LPARAM);
void SelectDLLFIle(UI,HWND);
void SetProcessComboBox(UI);
BOOL InjectDLL(UI,HWND);
BOOL EjectDLL(UI, HWND);
BOOL SetPrivilege(LPCTSTR, BOOL);
DWORD GetProcessPID(char*);
#endif