#pragma once
#include <string>
#include <sstream>
#include <algorithm>
#include <cctype>
#include <vector>
#include <iostream>

namespace Utility
{
	template<class T> std::string to_str(T element)
	{
		std::stringstream ss;
		ss << element;
		return ss.str();
	}

	std::string get_time_stamp();

}