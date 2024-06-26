#include "../inc/Server.hpp"

void	Server::handleQuit(int index)
{
	std::cout << CYAN << "- Client(" << index << ") quitting" << NC << std::endl;
	m_client[index].m_nick.clear();	
	m_client[index].m_user.clear();	
	m_client[index].m_name.clear();	
	m_client[index].m_ip.clear();	
	m_client[index].m_fd = -1;	
	m_client[index].m_auth = false;
}