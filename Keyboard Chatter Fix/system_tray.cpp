// LFInteractive LLC. (c) 2020-2024 All Rights Reserved
#include "system_tray.h"
#include <shellapi.h>
#include "resource.h"
#include <string>

#define IDM_EXIT 0x01167
#define IDM_OPEN_GITHUB 0x01168
#define NOTIFICATION_TRAY_ICON_MESSAGE (WM_USER + 1)
NOTIFYICONDATAW nid;

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
					AppendMenuW(hPopupMenu, MF_STRING, IDM_EXIT, L"Exit");

					TrackPopupMenu(hPopupMenu, TPM_LEFTALIGN | TPM_RIGHTBUTTON, pt.x, pt.y, 0, hwnd, NULL);

					break;
				}
				case WM_LBUTTONDOWN:
				{
					if (hPopupMenu != nullptr)
					{
						DestroyMenu(hPopupMenu);
						hPopupMenu = nullptr;
					}
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
					//Handle Open GitHub option
					ShellExecuteW(NULL, L"open", L"https://github.com/drew-chase/keyboard-chatter-fix", NULL, NULL, SW_SHOWNORMAL);
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

		case WM_DESTROY:
		{
			// Clean up resources when the window is destroyed
			if (hPopupMenu != nullptr)
			{
				DestroyMenu(hPopupMenu);
				hPopupMenu = nullptr;
			}
			PostQuitMessage(0);
			break;
		}

		case WM_CLOSE:
		{
			if (hPopupMenu != nullptr)
			{
				DestroyMenu(hPopupMenu);
				hPopupMenu = nullptr;
			}
		}

		default:
			return DefWindowProcW(hwnd, message, w_param, l_param);
	}
}

HWND system_tray::CreateHiddenWindow()
{
	HINSTANCE hInstance = GetModuleHandle(NULL);

	// Register a window class for the hidden window
	WNDCLASS wc = { 0 };
	wc.lpfnWndProc = WndProc;
	wc.hInstance = hInstance;
	wc.lpszClassName = L"MyHiddenWindowClass";
	RegisterClass(&wc);

	// Create a hidden window
	return CreateWindowExW(0, L"MyHiddenWindowClass", L"", 0, 0, 0, 0, 0, HWND_MESSAGE, NULL, hInstance, NULL);
}

system_tray::system_tray()
{
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