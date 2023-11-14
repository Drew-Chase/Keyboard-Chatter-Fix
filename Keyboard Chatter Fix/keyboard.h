// LFInteractive LLC. (c) 2020-2024 All Rights Reserved
#pragma once
#include "configuration.h"
#include <Windows.h>
class keyboard
{
public:
	/// <summary>
	/// Creates the keyboard hook.
	/// </summary>
	keyboard(configuration* config);
	/// <summary>
	/// Unhooks the keyboard hook.
	/// </summary>
	~keyboard();
};
