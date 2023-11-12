// LFInteractive LLC. (c) 2020-2024 All Rights Reserved
#include "keyboard.h"
#include "system_tray.h"

int WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nShowCmd)
{
	auto tray = new system_tray(); // Creates the system tray icon
	auto key = new keyboard(); // Creates the keyboard hook
	delete(tray); // Deletes the system tray
	delete(key); // Deletes the keyboard hook
	return 0;
}