#include "../inc/Server.hpp"

void	Server::handleUser(int index)
{
	if (m_client[index].m_auth < 2)
	return ;
	std::string user = m_client[index].m_buffer.front();
	m_client[index].m_user = user;
	popNonStop(index, 2);
	std::string ip = m_client[index].m_buffer.front();
	m_client[index].m_ip = ip;
	popNonStop(index, 1);
	std::string name = m_client[index].m_buffer.front();
	m_client[index].m_name = name;
	popNonStop(index, 1);
	std::cout << CYAN << "- Client(" << index << ") using '" << user << "@" << name << "' as USER@NAME" << NC << std::endl;
	// welcome message
	sendMsgClient(index, ":toxic.irc 001 " + m_client[index].m_nick + " :Welcome to the TOXIC IRC Network!");
	sendMsgClient(index, ":toxic.irc 002 " + m_client[index].m_nick + " :Your host is toxic.irc, running version 1.0");
	sendMsgClient(index, ":toxic.irc 003 " + m_client[index].m_nick + " :This server was created today");
	sendMsgClient(index, ":toxic.irc 004 " + m_client[index].m_nick + " :Stay Toxic in this server or you will be banned by Teemo or Singed");
	// simulating a channel
	//joinChannel(index, "Botlane");
	m_client[index].m_auth = 3;
}