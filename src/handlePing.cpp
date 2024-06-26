#include "../inc/Server.hpp"

void	Server::handlePing(int index)
{
	std::string token = m_client[index].m_buffer.front();
	sendMsgClient(index, "PONG " + token);
	std::cout << CYAN << "PONG " << token << NC << std::endl;
}