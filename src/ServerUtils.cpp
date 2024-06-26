#include "../inc/Server.hpp"

void	Server::sendMsgClient(int client, std::string msg)
{
	send(m_fds[client].fd, (msg + "\r\n").c_str() , msg.size() + 2 , 0);
}

void	Server::popNonStop(int index, int num)
{
	for (int i = 0; i < num; ++i)
	m_client[index].m_buffer.pop_front();
}

bool	Server::checkNick(std::string nick)
{
	for (int i = 1; i <= MAX_CLIENTS; ++i)
	{
	if (m_client[i].m_nick == nick)
		return (true);
	}
	return (false);	 
}

std::string	Server::removeBeginningChar(std::string str, char c)
{
	int i = 0;

	while (str[i])
	{
		if (str[i] != c)
			break ;
		i++;
	}
	return (&str[i]);
}

std::string	Server::Deque2String(std::deque<std::string> buffer)
{
	std::string msg;
    for (std::deque<std::string>::iterator it = buffer.begin(); it != buffer.end(); ++it)
	{
        msg += *it + " ";
	}
	return (msg);
}