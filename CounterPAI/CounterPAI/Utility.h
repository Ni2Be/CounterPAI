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

	template<class T> T from_str(const std::string& str)
	{
		std::stringstream ss;
		ss << str;
		T ret;
		ss >> ret;
		return ret; 
	}

	std::string get_time_stamp();

}