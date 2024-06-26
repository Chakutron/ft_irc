#include "../inc/Server.hpp"

void Server::handleMode(int index) 
{
	std::string key;
	std::string operatorNick;
	std::string limit;

	//if the client is not an IRC operator, send an error message
	// if (m_client[index].m_auth < 3) {
	// 	sendMsgClient(index, ":toxic.irc 481 " + m_client[index].m_nick + " :Permission Denied- You're not an IRC operator");
	// 	return;
	// }

	if (m_client[index].m_buffer.size() < 2) 
	{
		sendMsgClient(index, ":toxic.irc 461 " + m_client[index].m_nick + " MODE :Not enough parameters");
		return;
	}

	std::string channel = m_client[index].m_buffer.front();
	popNonStop(index, 1);
	std::string modeChange = m_client[index].m_buffer.front();
	popNonStop(index, 1);

	// we have to handle the mode change when the mode is '+'
	if (modeChange[0] == '+') 
	{
		if (modeChange.size() < 2) 
		{
			sendMsgClient(index, ":toxic.irc 461 " + m_client[index].m_nick + " MODE :Not enough parameters");
			return;
		}

		char mode = modeChange[1];
		switch (mode) 
		{
			case 'i':
				sendMsgClient(index, ":" + m_client[index].m_nick + " MODE #" + channel + " +i");
				break;
			case 't':
				sendMsgClient(index, ":" + m_client[index].m_nick + " MODE #" + channel + " +t");
				break;
			case 'k':
				if (m_client[index].m_buffer.empty()) 
				{
					sendMsgClient(index, ":toxic.irc 461 " + m_client[index].m_nick + " MODE :Not enough parameters");
					return;
				}
				key = m_client[index].m_buffer.front();
				popNonStop(index, 1);
				sendMsgClient(index, ":" + m_client[index].m_nick + " MODE #" + channel + " +k " + key);
				break;
			case 'o':
				if (m_client[index].m_buffer.empty()) 
				{
					sendMsgClient(index, ":toxic.irc 461 " + m_client[index].m_nick + " MODE :Not enough parameters");
					return;
				}
				operatorNick = m_client[index].m_buffer.front();
				popNonStop(index, 1);
				sendMsgClient(index, ":" + m_client[index].m_nick + " MODE #" + channel + " +o " + operatorNick);
				break;
			case 'l':
				if (m_client[index].m_buffer.empty()) 
				{
					sendMsgClient(index, ":toxic.irc 461 " + m_client[index].m_nick + " MODE :Not enough parameters");
					return;
				}
				limit = m_client[index].m_buffer.front();
				popNonStop(index, 1);
				sendMsgClient(index, ":" + m_client[index].m_nick + " MODE #" + channel + " +l " + limit);
				break;
			default:
				sendMsgClient(index, ":toxic.irc 501 " + m_client[index].m_nick + " :Unknown MODE flag");
				break;
		}
	}

	// we have to handle the mode change when the mode is '-'
	else if (modeChange[0] == '-')
	{
		if (modeChange.size() < 2)
		{
			sendMsgClient(index, ":toxic.irc 461 " + m_client[index].m_nick + " MODE :Not enough parameters");
			return;
		}

		char mode = modeChange[1];
		switch (mode) 
		{
			case 'i':
				sendMsgClient(index, ":" + m_client[index].m_nick + " MODE #" + channel + " -i");
				break;
			case 't':
				sendMsgClient(index, ":" + m_client[index].m_nick + " MODE #" + channel + " -t");
				break;
			case 'k':
				sendMsgClient(index, ":" + m_client[index].m_nick + " MODE #" + channel + " -k");
				break;
			case 'o':
				if (m_client[index].m_buffer.empty())
				{
					sendMsgClient(index, ":toxic.irc 461 " + m_client[index].m_nick + " MODE :Not enough parameters");
					return;
				}
				operatorNick = m_client[index].m_buffer.front();
				popNonStop(index, 1);
				sendMsgClient(index, ":" + m_client[index].m_nick + " MODE #" + channel + " -o " + operatorNick);
				break;
			case 'l':
				sendMsgClient(index, ":" + m_client[index].m_nick + " MODE #" + channel + " -l");
				break;
			default:
				sendMsgClient(index, ":toxic.irc 501 " + m_client[index].m_nick + " :Unknown MODE flag");
				break;
		}
	}
}