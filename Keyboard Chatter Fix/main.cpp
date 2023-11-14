// LFInteractive LLC. (c) 2020-2024 All Rights Reserved
#include "keyboard.h"
#include "system_tray.h"
#include "configuration.h"
int WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nShowCmd)
{
	auto config = new configuration();
	auto tray = new system_tray(config); // Creates the system tray icon
	auto key = new keyboard(config); // Creates the keyboard hook
	delete(tray); // Deletes the system tray
	delete(key); // Deletes the keyboard hook
	delete(config); // Deletes the keyboard hook
	return 0;
}