// LFInteractive LLC. (c) 2020-2024 All Rights Reserved
#include <Windows.h>
#include <shellapi.h>
#include <string>
#include "resource.h"

#define IDM_EXIT 0x01167
#define IDM_OPEN_GITHUB 0x01168
#define NOTIFICATION_TRAY_ICON_MESSAGE (WM_USER + 1)

// Global Variables.
ULONGLONG time_since_last_keypress;
DWORD last_keypress;
NOTIFYICONDATAW nid;

LRESULT CALLBACK KeyboardProc(int passthrough, WPARAM state, LPARAM flag)
{
	if (state == WM_KEYDOWN || state == WM_SYSKEYDOWN /* When is this ever called???? */) // If the key is pressed
	{
		auto key = (KBDLLHOOKSTRUCT*)flag; // Get the key pressed
		DWORD code = key->vkCode; // Get the key code
		ULONGLONG time = GetTickCount64() - time_since_last_keypress; // Get the time since the last keypress

		if (time < 50 && last_keypress == code) // If the time is less than 50ms and the last keypress is the same as the current keypress
		{
			return 1; // Return 1 to block the keypress
		}

		time_since_last_keypress = GetTickCount64(); // Set the time since the last keypress
		last_keypress = code; // Set the last keypress
	}

	return CallNextHookEx(NULL, passthrough, state, flag); // Passes the keypress to the next hook
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM w_param, LPARAM l_param)
{
	static HMENU hPopupMenu = nullptr;

	switch (message)
	{
		case NOTIFICATION_TRAY_ICON_MESSAGE:
			switch (l_param)
			{
				case WM_RBUTTONUP:
				{
					POINT pt;
					GetCursorPos(&pt);

					hPopupMenu = CreatePopupMenu();
					AppendMenuW(hPopupMenu, MF_STRING, IDM_OPEN_GITHUB, L"Open GitHub");
					AppendMenuW(hPopupMenu, MF_STRING, IDM_EXIT, L"Exit");

					TrackPopupMenu(hPopupMenu, TPM_LEFTALIGN | TPM_RIGHTBUTTON, pt.x, pt.y, 0, hwnd, NULL);

					break;
				}
			}
			break;

		case WM_COMMAND:
			switch (LOWORD(w_param))
			{
				case IDM_EXIT:
					// Handle Exit option
					PostQuitMessage(0);
					break;

				case IDM_OPEN_GITHUB:
					// Handle Open GitHub option
					ShellExecuteW(NULL, L"open", L"https://github.com/drew-chase/keyboard-chatter-fix", NULL, NULL, SW_SHOWNORMAL);
					break;
			}

			// Close the menu after an item is selected
			if (hPopupMenu != nullptr)
			{
				DestroyMenu(hPopupMenu);
				hPopupMenu = nullptr;
			}
			break;

		case WM_DESTROY:
			// Clean up resources when the window is destroyed
			if (hPopupMenu != nullptr)
			{
				DestroyMenu(hPopupMenu);
				hPopupMenu = nullptr;
			}
			PostQuitMessage(0);
			break;

		default:
			return DefWindowProc(hwnd, message, w_param, l_param);
	}

	return 0;
}

HWND create_hidden_window()
{
	HINSTANCE hInstance = GetModuleHandle(NULL);

	// Register a window class for the hidden window
	WNDCLASS wc = { 0 };
	wc.lpfnWndProc = WndProc;
	wc.hInstance = hInstance;
	wc.lpszClassName = L"MyHiddenWindowClass";
	RegisterClass(&wc);

	// Create a hidden window
	return CreateWindowEx(0, L"MyHiddenWindowClass", L"", 0, 0, 0, 0, 0, HWND_MESSAGE, NULL, hInstance, NULL);
}

bool create_system_tray_icon()
{
	nid = { 0 };
	nid.cbSize = sizeof(NOTIFYICONDATAW);
	auto icon = LoadIconW(GetModuleHandle(NULL), MAKEINTRESOURCEW(IDI_MAINICON)); // Load the MAINICON resource

	if (icon == NULL)
	{
		MessageBoxW(NULL, L"Unable to load icon from application resources", L"Error", MB_OK | MB_ICONERROR);
		return false;
	}

	nid.hWnd = create_hidden_window();
	nid.uID = 100;
	nid.uCallbackMessage = NOTIFICATION_TRAY_ICON_MESSAGE;
	nid.hIcon = icon;
	wcscpy_s(nid.szTip, L"Keyboard Chatter Fix");
	nid.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP | NIF_SHOWTIP;

	return Shell_NotifyIconW(NIM_ADD, &nid);
}

bool delete_system_tray_icon()
{
	return	Shell_NotifyIconW(NIM_DELETE, &nid);
}

int WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nShowCmd)
{
	HHOOK hook = SetWindowsHookExW(WH_KEYBOARD_LL, KeyboardProc, NULL, NULL);
	if (hook == NULL)
	{
		// If the keyboard hook fails, show an error message
		MessageBoxW(NULL, L"Unable to hook the keyboard input... sowwy?", L"Error", MB_OK | MB_ICONERROR);
		return 1;
	}

	// Attempts to create the system tray icon once the hook has been properly setup.
	if (!create_system_tray_icon())
	{
		auto error = GetLastError();
		MessageBoxW(NULL, (L"Unable to create the system tray icon. Error code: " + std::to_wstring(error)).c_str(), L"Error", MB_OK | MB_ICONERROR);
		return 1;
	}

	// Pauses the current thread waiting until keyboard hook message is received.
	MSG msg;
	if (GetMessageW(&msg, NULL, NULL, NULL) != 0)
	{
		TranslateMessage(&msg);
		DispatchMessageW(&msg);
	}

	bool unhook = UnhookWindowsHookEx(hook); // Attempts to unhook the keyboard hook

	if (!unhook)
	{
		// If unsuccessful show message box.
		MessageBoxW(NULL, L"Unable to unhook the keyboard input... sowwy?", L"Error", MB_OK | MB_ICONERROR);
		delete_system_tray_icon();
		return 1;
	}

	if (!delete_system_tray_icon())// Attempts to dispose of the system tray icon.
	{
		MessageBoxW(NULL, L"Unable to create the system tray icon...", L"Error", MB_OK | MB_ICONERROR);
	}

	return 0;
}