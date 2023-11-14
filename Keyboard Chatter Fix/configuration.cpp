// LFInteractive LLC. (c) 2020-2024 All Rights Reserved
#include <Windows.h>
#include "configuration.h"
#include <filesystem>
#include <fstream>
#include <string>
#include <iostream>
using std::string;

configuration::configuration()
{
	if (!std::filesystem::exists(config_file))
	{
		save();
	}
	else
	{
		load();
	}
}

configuration::~configuration()
{
	save();
}

void configuration::load()
{
	std::ifstream file(config_file);

	string line;
	while (std::getline(file, line))
	{
		if (line.find("delay=") != string::npos)
		{
			auto delay = line.substr(6);
			unsigned long long i = time_between_keypresses;
			try
			{
				i = std::stoull(delay);
			}
			catch (std::exception e)
			{
				MessageBoxW(NULL, L"Unable to parse configuration file. Reseting it...", L"Error", MB_OK | MB_ICONERROR);
				save();
				break;
			}
			time_between_keypresses = i;
			break;
		}
	}

	file.close();
}

void configuration::save()
{
	std::ofstream file(config_file);
	file << "# The time between key presses in milliseconds\n";
	file << "delay=";
	file << time_between_keypresses;
	file << " # default is 50";
	file.close();
}