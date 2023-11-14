// LFInteractive LLC. (c) 2020-2024 All Rights Reserved
#pragma once
class configuration
{
private:
	const char* config_file = "./config.ini";
public:
	unsigned long long time_between_keypresses = 50;
	configuration();
	~configuration();
	void load();
	void save();
};
