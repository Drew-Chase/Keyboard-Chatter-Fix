// LFInteractive LLC. (c) 2020-2024 All Rights Reserved
#pragma once
#include <Windows.h>
#include "configuration.h"
class system_tray
{
private:
	HWND CreateHiddenWindow();
public:
	/// <summary>
	/// Creates the system tray icon and the menu items.
	/// </summary>
	system_tray(configuration* config);

	/// <summary>
	/// Destroys the system tray icon.
	/// </summary>
	~system_tray();
};
