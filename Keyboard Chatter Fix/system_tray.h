// LFInteractive LLC. (c) 2020-2024 All Rights Reserved
#pragma once
#include <Windows.h>
class system_tray
{
private:
	HWND CreateHiddenWindow();
public:
	system_tray();
	~system_tray();
};
