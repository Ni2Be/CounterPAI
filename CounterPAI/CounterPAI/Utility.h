#pragma once
#include <string>
#include <sstream>

namespace Utility
{
	template<class T> std::string to_str(T element)
	{
		std::stringstream ss;
		ss << element;
		return ss.str();
	}
}