// LFInteractive LLC. (c) 2020-2024 All Rights Reserved
#include "keyboard.h"

// Global Variables.
ULONGLONG time_since_last_keypress;
DWORD last_keypress;
HHOOK hook;
byte press_count = 0;
configuration* cfg;

LRESULT CALLBACK KeyboardProc(int passthrough, WPARAM state, LPARAM flag)
{
	if (state == WM_KEYDOWN || state == WM_SYSKEYDOWN /* When is this ever called???? */) // If the key is pressed
	{
		auto key = (KBDLLHOOKSTRUCT*)flag; // Get the key pressed
		DWORD code = key->vkCode; // Get the key code
		ULONGLONG time = GetTickCount64() - time_since_last_keypress; // Get the time since the last keypress
		if (last_keypress == code)
		{
			press_count++; // Increment the press count
			if (time < cfg->time_between_keypresses)
			{
				if (press_count <= 8) // If the time is less than 50ms and the last keypress is the same as the current keypress
				{
					return 1; // Return 1 to block the keypress
				}
			}
		}
		else
		{
			press_count = 0; // Reset the press count
		}

		time_since_last_keypress = GetTickCount64(); // Set the time since the last keypress
		last_keypress = code; // Set the last keypress
	}

	return CallNextHookEx(NULL, passthrough, state, flag); // Passes the keypress to the next hook
}

keyboard::keyboard(configuration* config)
{
	cfg = config;
	hook = SetWindowsHookExW(WH_KEYBOARD_LL, KeyboardProc, NULL, NULL); // Sets the keyboard hook
	if (hook == NULL)
	{
		// If the keyboard hook fails, show an error message
		MessageBoxW(NULL, L"Unable to hook the keyboard input... sowwy?", L"Error", MB_OK | MB_ICONERROR);
	}
	// Pauses the current thread waiting until keyboard hook message is received.
	MSG msg;
	while (GetMessageW(&msg, NULL, NULL, NULL) > 0)
	{
		TranslateMessage(&msg);
		DispatchMessageW(&msg);
	}
}

keyboard::~keyboard()
{
	bool unhook = UnhookWindowsHookEx(hook); // Attempts to unhook the keyboard hook

	if (!unhook)
	{
		// If unsuccessful show message box.
		MessageBoxW(NULL, L"Unable to unhook the keyboard input... sowwy?", L"Error", MB_OK | MB_ICONERROR);
	}
}