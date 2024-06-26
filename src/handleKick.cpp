#include "../inc/Server.hpp"

void Server::handleKick(int index)
{
	if (m_client[index].m_auth < 3)
		return;

	std::string channel = m_client[index].m_buffer.front();
	popNonStop(index, 1);
	std::string nickname = m_client[index].m_buffer.front();
	popNonStop(index, 1);

	// we have to find the client to kick by nickname in the specified channel
	for (int i = 1; i <= MAX_CLIENTS; ++i)
	{
		if (m_client[i].m_nick == nickname)
		{
			// disconnect the client
			close(m_fds[i].fd);
			m_fds[i].fd = -1;
			// we have to send appropriate messages to affected clients
			sendMsgClient(index, "KICK #" + channel + " " + nickname + " :You have been kicked from the channel");
			return;
		}
	}

	// if client is not found, send error message
	sendMsgClient(index, ":toxic.irc 401 " + m_client[index].m_nick + " " + nickname + " :No such nick/channel");
}