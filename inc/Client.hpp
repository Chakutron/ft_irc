#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <string>

class Client
{
public:
	int fd;
	std::string nickname;
	std::string username;
	std::string realname;
	bool authenticated;
	bool isRegistered;
	bool isOperator;
	// bool passwordAccepted;
	// bool nickSet;
	// bool userInfoSet;

	Client(int fd);
	~Client();
};

#endif // CLIENT_HPP