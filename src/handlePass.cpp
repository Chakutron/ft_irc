#include "../inc/Server.hpp"

void	Server::handlePass(int index)
{
	std::string tmp;
	tmp = m_client[index].m_buffer.front();
	popNonStop(index, 1);
	if (tmp[0] == ':')
	tmp = &tmp[1];
	if (tmp != m_pwd)
	{
	std::cout << RED << "- ERROR: password not valid '" << tmp << "' on Client(" << index << ")" << NC << std::endl;
	sendMsgClient(index, ":toxic.irc 464 client :Password incorrect");
	// we have to close connection
	return ;
	}
	std::cout << CYAN << "- Client(" << index << ") connected with the valid password" << NC << std::endl;
	sendMsgClient(index, ":toxic.irc 376 client :End of /MOTD command");
	m_client[index].m_auth = 1;
}