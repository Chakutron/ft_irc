#include "../inc/Server.hpp"

void	Server::handleNick(int index)
{
	if (m_client[index].m_auth < 1)
	return ;
	// we have to verify that nickname is not used
	std::string nick = m_client[index].m_buffer.front();
	if (checkNick(nick) == false)
	{
	m_client[index].m_nick = nick;
	popNonStop(index, 1);
	std::cout << CYAN << "- Client(" << index << ") using '" << nick << "' as NICK" << NC << std::endl;
	//sendMsgClient(index, ":toxic.irc PRIVMSG " + m_client[index].m_nick + " :You are now known as " + m_client[index].m_nick);
	m_client[index].m_auth = 2;
	}
	else
	{
	std::cout << RED << "- Client(" << index << ") using a existing nickname: '" << nick << "'" << NC << std::endl;
	sendMsgClient(index, ":toxic.irc 433 * " + nick + " :Nickname is already in use");
	}
}