#pragma once

#include <string>
#include <iomanip>
#include <sstream>

using std::string;

class Utils
{
public:

	template <typename T>
	static string to_string_with_precision(const T a_value, const int precision = 6)
	{
		std::ostringstream out;
		out << std::setprecision(precision) << a_value;
		return out.str();
	}

};
