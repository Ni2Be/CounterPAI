#include "Debug_Log.h"

#include "date.h"
#include <iostream>
#include <fstream>

Debug_Log::Debug_Log()
{
	using namespace date;
	using namespace std::chrono;
	auto start_time = floor<seconds>(system_clock::now());
	std::stringstream ss;
	ss << start_time;
	
	std::string s = ss.str();
	std::replace(s.begin(), s.end(), ':', '-');
	
	m_file_name = "logs/Log " + s + ".txt";

	std::cout << "\nLog file: " << m_file_name;
}

void Debug_Log::log(const std::string& message) const
{
	using namespace date;
	using namespace std::chrono;
	auto time = floor<seconds>(system_clock::now());
	std::stringstream ss;
	ss << time;

	std::ofstream debug_file(m_file_name, std::ofstream::app);
	if (!debug_file)
		std::cerr << "\ncould not open debug log: " << m_file_name;
	debug_file << "\n-----------------\n" << ss.str() << "\n" << message;
}

