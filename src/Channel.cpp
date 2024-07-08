#include "../inc/Channel.hpp"
#include <algorithm>

Channel::Channel(const std::string& name) : name(name), invite_only(false), open_topic(false), user_limit(0)  {}

Channel::~Channel() {}

void Channel::addClient(Client* client)
{
	clients.push_back(client);
}

void Channel::removeClient(Client* client)
{
	clients.erase(std::remove(clients.begin(), clients.end(), client), clients.end());
	removeOperator(client);
}

bool Channel::isOperator(Client* client) const
{
	return std::find(operators.begin(), operators.end(), client) != operators.end();
}

void Channel::addOperator(Client* client)
{
	if (!isOperator(client))
	{
		operators.push_back(client);
	}
}

void Channel::removeOperator(Client* client)
{
	operators.erase(std::remove(operators.begin(), operators.end(), client), operators.end());
}