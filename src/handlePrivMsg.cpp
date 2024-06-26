#include "../inc/Server.hpp"

void	Server::handlePrivMsg(int index)
{
	std::string user = m_client[index].m_buffer.front();
	popNonStop(index, 1);
	std::string msg = Deque2String(m_client[index].m_buffer);
	for (int i = 1; i <= MAX_CLIENTS; ++i)
	{
		if (m_client[i].m_nick == user)
		{
			sendMsgClient(i, ":" + m_client[index].m_nick + "!" + m_client[index].m_user + "@" + m_client[index].m_ip + " PRIVMSG " + user + " " + msg);
			break ;
		}
	}
}