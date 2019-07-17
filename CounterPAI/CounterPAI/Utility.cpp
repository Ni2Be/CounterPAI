#include "Utility.h"

#include "date.h"
#include <iostream>
#include <fstream>


std::string Utility::get_time_stamp()
{
	using namespace date;
	using namespace std::chrono;
	auto time_point = floor<seconds>(system_clock::now());
	std::stringstream ss;
	ss << time_point;

	return ss.str();
}