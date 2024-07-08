#ifndef CHANNEL_HPP
#define CHANNEL_HPP

#include <string>
#include <vector>
#include "Client.hpp"

class Channel
{
public:
	std::string name;
	std::string topic;
	std::vector<Client*> clients;
	std::vector<Client*> operators;
	bool invite_only;
	bool open_topic;
	std::string key;
	int user_limit;

	Channel(const std::string& name);
	~Channel();

	void addClient(Client* client);
	void removeClient(Client* client);
	bool isOperator(Client* client) const;
	void addOperator(Client* client);
	void removeOperator(Client* client);
};

#endif // CHANNEL_HPP