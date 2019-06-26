#pragma once
#include <string>

class Debug_Log
{
public:
	Debug_Log();
	
	void log(const std::string& message) const;

	std::string m_file_name;
};

