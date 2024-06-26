#include "../inc/Server.hpp"

void Server::handleInvite(int index) 
{
	//if the client is not an IRC operator, send an error message
	// if (m_client[index].m_auth < 3) {
	// 	sendMsgClient(index, ":toxic.irc 481 " + m_client[index].m_nick + " :Permission Denied- You're not an IRC operator");
	// 	return;
	// }

	// we have to check if the client is in a channel
	if (m_client[index].m_buffer.size() < 2) {
		sendMsgClient(index, ":toxic.irc 461 " + m_client[index].m_nick + " INVITE :Not enough parameters");
		return;
	}

	std::string nickname = m_client[index].m_buffer.front();
	popNonStop(index, 1);
	std::string channel = m_client[index].m_buffer.front();
	popNonStop(index, 1);

	bool clientFound = false;
	int inviteeIndex = -1;
	//we have to find the client to invite by nickname
	for (int i = 1; i <= MAX_CLIENTS; ++i) 
	{
		if (m_client[i].m_nick == nickname) 
		{
			clientFound = true;
			inviteeIndex = i;
			break;
		}
	}

	//if the client is not found, send an error message
	if (!clientFound) 
	{
		sendMsgClient(index, ":toxic.irc 401 " + m_client[index].m_nick + " " + nickname + " :No such nick/channel");
		return;
	}

	sendMsgClient(inviteeIndex, ":" + m_client[index].m_nick + " INVITE " + nickname + " #" + channel);
	sendMsgClient(index, ":toxic.irc 341 " + m_client[index].m_nick + " " + nickname + " #" + channel + " :Invite to channel sent");
}