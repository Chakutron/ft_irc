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
#endif