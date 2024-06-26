#include "../inc/Server.hpp"

void	Server::handleJoin(int index)
{
	joinChannel(index, removeBeginningChar(m_client[index].m_buffer.front(), '#'));
}

void	Server::joinChannel(int index, std::string channel)
{
	// we have to check if the channel exists or not..

	sendMsgClient(index, m_client[index].m_nick + "!" + m_client[index].m_user + "@" + m_client[index].m_ip + " JOIN #" + channel);
	if (channel == "botlane")
	{
		std::cout << CYAN << "- Client(" << index << ") joined channel: " << channel << NC << std::endl;
		sendMsgClient(index, ":" + m_client[index].m_nick + "!" + m_client[index].m_user + "@" + m_client[index].m_ip + " JOIN :#" + channel);
		// we have to work with the channel's MOTD
		sendMsgClient(index, ":toxic.irc 332 " + m_client[index].m_nick + " #" + channel + " :Welcome to channel " + channel);
		// we have to list the users in the channel
		sendMsgClient(index, ":toxic.irc 353 " + m_client[index].m_nick + " = #" + channel + " :@Teemo @Singed Ashe Soraka");
		sendMsgClient(index, ":toxic.irc 366 " + m_client[index].m_nick + " #" + channel + " :End of NAMES list");
	}
}