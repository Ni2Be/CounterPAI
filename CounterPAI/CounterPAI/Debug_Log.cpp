#pragma once
#include "Debug_Log.h"

#include "date.h"
#include <iostream>
#include <fstream>
#include "Folder_Dialog.h"
#include "Utility.h"

Debug_Log::Debug_Log()
{
	std::string s = Utility::get_time_stamp();
	std::replace(s.begin(), s.end(), ':', '-');
	
	m_file_name = Folder_Dialog::get_exe_path() + "/logs/Log " + s + ".txt";
	std::replace(m_file_name.begin(), m_file_name.end(), '\\', '/');

	std::cout << "\nLog file: " << m_file_name;
}

void Debug_Log::log(const std::string& message) const
{
	std::ofstream debug_file(m_file_name, std::ofstream::app);
	if (!debug_file)
		std::cerr << "\ncould not open debug log: " << m_file_name;
	debug_file << "\n-----------------\n" << Utility::get_time_stamp() << "\n" << message;
}

