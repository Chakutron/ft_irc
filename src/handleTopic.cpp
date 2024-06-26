#include "../inc/Server.hpp"

void Server::handleTopic(int index) 
{
	//if the client is not an IRC operator, send an error message

	// if (m_client[index].m_auth < 3) 
	// {
	// 	sendMsgClient(index, ":toxic.irc 481 " + m_client[index].m_nick + " :Permission Denied- You're not an IRC operator");
	// 	return;
	// }

	//we have to check if the client is in a channel
	if (m_client[index].m_buffer.size() < 1) 
	{
		// 461 ERR_NEEDMOREPARAMS
		sendMsgClient(index, ":toxic.irc 461 " + m_client[index].m_nick + " TOPIC :Not enough parameters");
		return;
	}

	std::string channel = removeBeginningChar(m_client[index].m_buffer.front(), '#');
	popNonStop(index, 1);

	if (m_client[index].m_buffer.empty()) 
	{
		// 332 RPL_TOPIC
		std::string currentTopic = "Welcome to #" + channel;
		sendMsgClient(index, ":toxic.irc 332 " + m_client[index].m_nick + " #" + channel + " :" + currentTopic);
	}
	else if (m_client[index].m_buffer.front() == ":") 
	{
		popNonStop(index, 1);

		if (m_client[index].m_buffer.empty()) 
		{
			// 412 ERR_NOTEXTTOSEND
			sendMsgClient(index, ":toxic.irc 412 " + m_client[index].m_nick + " :No text to set");
			return;
		}

		std::string newTopic = m_client[index].m_buffer.front();
		popNonStop(index, 1);

		sendMsgClient(index, ":" + m_client[index].m_nick + " TOPIC #" + channel + " :" + newTopic);
	}
	else
		// 461 ERR_NEEDMOREPARAMS
		sendMsgClient(index, ":toxic.irc 461 " + m_client[index].m_nick + " :Not enough parameters");
}