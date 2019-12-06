#pragma once
#include "Utility.h"

#include "date.h"
#include <iostream>
#include <fstream>

#define NOMINMAX
#include <windows.h>

std::string Utility::get_time_stamp()
{
	using namespace date;
	using namespace std::chrono;
	auto time_point = floor<seconds>(system_clock::now());
	std::stringstream ss;
	ss << time_point;

	return ss.str();
}

bool Utility::memory_check(int counter)
{
	bool should_continue = true;
	MEMORYSTATUSEX status;
	status.dwLength = sizeof(status);
	GlobalMemoryStatusEx(&status);
	std::cout << "\r" << counter << " elements, memory " << status.ullAvailPhys << "      ";
	if (status.ullAvailPhys < 0.1 * status.ullTotalPhys)
	{
		std::cerr << "\nnot enought memory\n";
		std::cout << "continue? y/n ";
		char con;
		std::cin >> con;
		should_continue = (con == 'n' ? false : true);
	}
	return should_continue;
}