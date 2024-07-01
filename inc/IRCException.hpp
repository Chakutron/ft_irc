#ifndef IRC_EXCEPTION_HPP
#define IRC_EXCEPTION_HPP

#include <string>
#include <stdexcept>

class IRCException : public std::runtime_error
{
public:
	IRCException(const std::string& message) : std::runtime_error(message) {}
};

#endif // IRC_EXCEPTION_HPP