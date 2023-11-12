// LFInteractive LLC. (c) 2020-2024 All Rights Reserved
#pragma once
#include <Windows.h>
class system_tray
{
private:
	HWND CreateHiddenWindow();
public:
	/// <summary>
	/// Creates the system tray icon and the menu items.
	/// </summary>
	system_tray();

	/// <summary>
	/// Destroys the system tray icon.
	/// </summary>
	~system_tray();
};
