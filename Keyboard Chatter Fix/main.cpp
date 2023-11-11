// LFInteractive LLC. (c) 2020-2024 All Rights Reserved
#include <Windows.h>
#include "resource.h"

ULONGLONG time_since_last_keypress;
DWORD last_keypress;

LRESULT CALLBACK KeyboardProc(int passthrough, WPARAM state, LPARAM flag)
{
	if (state == WM_KEYDOWN || state == WM_SYSKEYDOWN)
	{
		auto key = (KBDLLHOOKSTRUCT*)flag;
		DWORD code = key->vkCode;
		ULONGLONG time = GetTickCount64() - time_since_last_keypress;

		if (time < 50 && last_keypress == code)
		{
			return 1;
		}

		time_since_last_keypress = GetTickCount64();
		last_keypress = code;
	}

	return CallNextHookEx(NULL, passthrough, state, flag);
}

int main()
{
	HHOOK hook = SetWindowsHookExW(WH_KEYBOARD_LL, KeyboardProc, NULL, NULL);
	if (hook == NULL)
	{
		MessageBoxW(NULL, L"Unable to hook the keyboard input... sowwy?", L"Error", MB_OK | MB_ICONERROR);
		return 1;
	}

	MSG msg;
	if (GetMessageW(&msg, NULL, NULL, NULL) != 0)
	{
		TranslateMessage(&msg);
		DispatchMessageW(&msg);
	}

	bool unhook = UnhookWindowsHookEx(hook);

	if (!unhook)
	{
		MessageBoxW(NULL, L"Unable to unhook the keyboard input... sowwy?", L"Error", MB_OK | MB_ICONERROR);
		return 1;
	}

	return 0;
}