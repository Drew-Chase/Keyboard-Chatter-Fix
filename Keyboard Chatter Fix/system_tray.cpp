// LFInteractive LLC. (c) 2020-2024 All Rights Reserved
#include "system_tray.h"
#include <shellapi.h>
#include "resource.h"
#include <string>
#include <filesystem>

#define IDM_EXIT 0x01167
#define IDM_OPEN_GITHUB 0x01168
#define IDM_OPEN_CONFIG 0x01169
#define IDM_RELOAD_CONFIG 0x01170
#define NOTIFICATION_TRAY_ICON_MESSAGE (WM_USER + 1)
NOTIFYICONDATAW nid;
configuration* app_config;
LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM w_param, LPARAM l_param)
{
	static HMENU hPopupMenu = nullptr;

	switch (message)
	{
		case NOTIFICATION_TRAY_ICON_MESSAGE:
		{
			switch (l_param)
			{
				case WM_RBUTTONUP:
				{
					POINT pt;
					GetCursorPos(&pt);

					hPopupMenu = CreatePopupMenu();
					AppendMenuW(hPopupMenu, MF_STRING, IDM_OPEN_GITHUB, L"Open GitHub");
					AppendMenuW(hPopupMenu, MF_STRING, IDM_OPEN_CONFIG, L"Open Config");
					AppendMenuW(hPopupMenu, MF_STRING, IDM_RELOAD_CONFIG, L"Reload Config");
					AppendMenuW(hPopupMenu, MF_STRING, IDM_EXIT, L"Exit");

					TrackPopupMenu(hPopupMenu, TPM_LEFTALIGN | TPM_RIGHTBUTTON, pt.x, pt.y, 0, hwnd, NULL);

					break;
				}
			}
			break;
		}

		case WM_COMMAND:
		{
			switch (LOWORD(w_param))
			{
				case IDM_OPEN_GITHUB:
				{
					// Handle Open GitHub option
					ShellExecuteW(NULL, L"open", L"https://github.com/drew-chase/keyboard-chatter-fix", NULL, NULL, SW_SHOWNORMAL);
					break;
				}
				case IDM_OPEN_CONFIG:
				{
					// Handle Open GitHub option
					auto file = std::filesystem::absolute("./config.ini");
					ShellExecuteW(NULL, L"open", file.c_str(), L"", NULL, SW_SHOWNORMAL);
					break;
				}
				case IDM_RELOAD_CONFIG:
				{
					// Handle Open GitHub option
					app_config->load();
					break;
				}
				case IDM_EXIT:
				{
					// Handle Exit option
					PostQuitMessage(0);
					break;
				}
			}

			// Close the menu after an item is selected
			if (hPopupMenu != nullptr)
			{
				DestroyMenu(hPopupMenu);
				hPopupMenu = nullptr;
			}
			break;
		}

		default:
			return DefWindowProcW(hwnd, message, w_param, l_param);
	}
}

HWND system_tray::CreateHiddenWindow() // This is needed because windows is stupid and requires a window to be created to use the system tray.
{
	HINSTANCE hInstance = GetModuleHandle(NULL);

	// Register a window class for the hidden window
	const wchar_t* className = L"KeyboardChatterTrayWindow";
	WNDCLASS wc = { 0 };
	wc.lpfnWndProc = WndProc;
	wc.hInstance = hInstance;
	wc.lpszClassName = className;
	RegisterClass(&wc);

	HWND explorerHandle = FindWindow(L"Shell_TrayWnd", NULL); // Sets the parent window to the taskbar so the context menu will get destroyed when the tray loses focus.

	// Create a hidden window
	return CreateWindowExW(0, className, L"", 0, 0, 0, 0, 0, explorerHandle, NULL, hInstance, NULL);
}

system_tray::system_tray(configuration* config)
{
	app_config = config;
	nid = { 0 };
	nid.cbSize = sizeof(NOTIFYICONDATAW);
	auto icon = LoadIconW(GetModuleHandle(NULL), MAKEINTRESOURCEW(IDI_MAINICON)); // Load the MAINICON resource

	if (icon == NULL)
	{
		MessageBoxW(NULL, L"Unable to load icon from application resources", L"Error", MB_OK | MB_ICONERROR);
	}

	nid.hWnd = CreateHiddenWindow();
	nid.uID = 100;
	nid.uCallbackMessage = NOTIFICATION_TRAY_ICON_MESSAGE;
	nid.hIcon = icon;
	wcscpy_s(nid.szTip, L"Keyboard Chatter Fix");
	nid.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP | NIF_SHOWTIP;

	if (!Shell_NotifyIconW(NIM_ADD, &nid))
	{
		auto error = GetLastError();
		MessageBoxW(NULL, (L"Unable to create the system tray icon. Error code: " + std::to_wstring(error)).c_str(), L"Error", MB_OK | MB_ICONERROR);
	}
}

system_tray::~system_tray()
{
	if (!Shell_NotifyIconW(NIM_DELETE, &nid))
	{
		MessageBoxW(NULL, L"Unable to create the system tray icon...", L"Error", MB_OK | MB_ICONERROR);
	}
}