#ifndef STRING_UTILS_HPP
#define STRING_UTILS_HPP
#include <string>
#include <sstream>
namespace StringUtils 
{
template <typename T>
std::string toString(T value) {
	std::ostringstream oss;
	oss << value;
	return oss.str();
}
} // namespace StringUtils

std::string removeConsecutiveDuplicates(const std::string& str, char ch)
{
	std::string result;
	char lastChar = '\0';
	for (size_t i = 0; i < str.length(); ++i)
	{
		if (str[i] != ch || str[i] != lastChar)
		{
			result += str[i];
		}
		lastChar = str[i];
	}
	return result;
}
#endif