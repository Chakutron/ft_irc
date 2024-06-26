#include "../inc/Server.hpp"

Server::~Server() {}

Server::Server(int port, std::string pwd) : m_port(port), m_pwd(pwd)
{
	m_serverFD = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (m_serverFD < 0)
	{
		perror("socket() failed");
		exit(-1);
	}
	
	int on = 1;
	int rc = setsockopt(m_serverFD, SOL_SOCKET, SO_REUSEADDR, (char *)&on, sizeof(on));
	if (rc < 0)
	{
	perror("setsockopt() failed");
	close(m_serverFD);
	exit(-1);
	}

	rc = ioctl(m_serverFD, FIONBIO, (char *)&on);
	if (rc < 0)
	{
	perror("ioctl() failed");
	close(m_serverFD);
	exit(-1);
	}

	sockaddr_in serverAddress;
	serverAddress.sin_family = AF_INET;
	serverAddress.sin_port = htons(this->m_port);
	serverAddress.sin_addr.s_addr = INADDR_ANY;

	rc = bind(m_serverFD, (struct sockaddr*)&serverAddress, sizeof(serverAddress));
	if (rc < 0)
	{
	perror("bind() failed");
	close(m_serverFD);
	exit(-1);
	}

	rc = listen(m_serverFD, 10);
	if (rc < 0)
	{
	perror("listen() failed");
	close(m_serverFD);
	exit(-1);
	}

	//Initialize fds and set for every client
	memset(m_fds, 0, sizeof(m_fds));
	m_fds[0].fd = m_serverFD;
	m_fds[0].events = POLLIN;

	// Initialize commands map
	m_commandsMap["PASS"] = &Server::handlePass;
	m_commandsMap["USER"] = &Server::handleUser;
	m_commandsMap["NICK"] = &Server::handleNick;
	m_commandsMap["PING"] = &Server::handlePing;
	m_commandsMap["JOIN"] = &Server::handleJoin;
	m_commandsMap["QUIT"] = &Server::handleQuit;
	
	m_commandsMap["KICK"] = &Server::handleKick;
	m_commandsMap["INVITE"] = &Server::handleInvite;
	m_commandsMap["TOPIC"] = &Server::handleTopic;
	m_commandsMap["MODE"] = &Server::handleMode;

	m_commandsMap["PRIVMSG"] = &Server::handlePrivMsg;

}

void Server::runServ(int port)
{
	for (int i = 1; i < MAX_CLIENTS; i++)
	{
		m_fds[i].fd = -1;
		m_fds[i].events = POLLIN;
	}
	int numClients = 0;

	std::cout << GREEN << "*** TOXIC IRC 1.0 ***" << NC << std::endl;
	std::cout << YELLOW << "- Listening on port: " << port << NC << std::endl;
	//start of infinite loop
	while (1)
	{
	int rc = poll(m_fds, MAX_CLIENTS + 1, -1);
	if (rc < 0)
	{
		perror("poll() failed");
		break;
	}
	if (m_fds[0].revents & POLLIN) 
	{
		int newClientFD = accept(m_serverFD, NULL, NULL);
		if (newClientFD >= 0)
		{
		bool added = false;
		for (int i = 1; i <= MAX_CLIENTS; ++i)
		{
			if (m_fds[i].fd == -1)
			{
			m_fds[i].fd = newClientFD;
			m_fds[i].events = POLLIN;
			m_client[i].m_fd = newClientFD;
			numClients++;
			added = true;
			break;
			}
		}
		if (!added)
		{
			std::cerr << "Max clients reached, rejecting connection." << std::endl;
			close(newClientFD);
		}
		else
			std::cout << "\rClients connected: " << numClients << std::flush;
		}
		else
		perror("accept() failed");
	}
	
	for (int i = 1; i <= MAX_CLIENTS; ++i)
	{		 
		if (m_fds[i].fd != -1 && (m_fds[i].revents & POLLIN))
		{
		char buffer[1024];
		int bytesRead = recv(m_fds[i].fd, buffer, sizeof(buffer) - 1, 0);
		if (bytesRead < 0)
		{
			perror("recv() failed");
			close(m_fds[i].fd);
			m_fds[i].fd = -1;
			numClients--;
		}
		else if (bytesRead == 0)
		{
			std::cout << "\nClient disconnected: " << m_fds[i].fd << std::endl;
			close(m_fds[i].fd);
			m_fds[i].fd = -1;
			numClients--;
		}
		else
		{
			buffer[bytesRead] = '\0';
			std::cout << "\nReceived from client (" << i << "): " << buffer << std::endl;
			setBuffer(buffer, i);
		}
		}
	}
	}
	//end of infinite loop
	
	close(m_serverFD); 
}

void Server::setBuffer(const char *buffer, int index)
{
	if (buffer)
	m_client[index].m_tmpBuffer += buffer;
	if (validBuffer(m_client[index].m_tmpBuffer.c_str()) == true)
	{
	size_t pos = m_client[index].m_tmpBuffer.find("\r\n");
	if (pos != std::string::npos)
	{
		splitBuffer(index, m_client[index].m_tmpBuffer.substr(0, pos));
		m_client[index].m_tmpBuffer = m_client[index].m_tmpBuffer.substr(pos + 2);
		execBuffer(index);
		return ;
	}
	if (pos == std::string::npos)
		pos = m_client[index].m_tmpBuffer.find("\r");
	if (pos == std::string::npos)
		pos = m_client[index].m_tmpBuffer.find("\n");
	splitBuffer(index, m_client[index].m_tmpBuffer.substr(0, pos));
	m_client[index].m_tmpBuffer = m_client[index].m_tmpBuffer.substr(pos + 1);
	execBuffer(index);
	}
}

bool	Server::validBuffer(const char *buffer)
{
	int i = 0;
	while (buffer[i])
	{
	if (buffer[i] == '\r' && buffer[i + 1] == '\n')
		return(true);
	else if (buffer[i] == '\r')
		return(true);
	else if (buffer[i] == '\n')
		return(true);
	i++;
	}
	return (false);
}

void Server::splitBuffer(int index, std::string str)
{
	std::istringstream iss(str);
	std::string word;

	while (iss >> word)
		m_client[index].m_buffer.push_back(word);
}

void Server::execBuffer(int index)
{
	if (m_client[index].m_buffer.empty() == true || m_client[index].m_buffer.front() == "")
	{
		m_client[index].m_buffer.clear();
		return ;
	}
	std::string tmp;
	tmp = m_client[index].m_buffer.front();
	popNonStop(index, 1);

	if (m_commandsMap.find(tmp) != m_commandsMap.end())
	{
		CommandFunction func = m_commandsMap[tmp];
		(this->*func)(index);
	}
	else
		std::cout << RED << "- ERROR: command '" << tmp << "' not found on client (" << index << ")" << NC << std::endl;
	
	m_client[index].m_buffer.clear();
	//std::cout << YELLOW << "recursive call setBuffer\n" << VIOLET << m_client[index].m_tmpBuffer << NC << std::endl;
	setBuffer(NULL, index);
}

void	Server::sendMsgClient(int client, std::string msg)
{
	send(m_fds[client].fd, (msg + "\r\n").c_str() , msg.size() + 2 , 0);
}

void	Server::popNonStop(int index, int num)
{
	for (int i = 0; i < num; ++i)
	m_client[index].m_buffer.pop_front();
}

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

bool	Server::checkNick(std::string nick)
{
	for (int i = 1; i <= MAX_CLIENTS; ++i)
	{
	if (m_client[i].m_nick == nick)
		return (true);
	}
	return (false);	 
}

void	Server::handleNick(int index)
{
	if (m_client[index].m_auth < 1)
	return ;
	// we have to verify that nickname is not used
	std::string nick = m_client[index].m_buffer.front();
	if (checkNick(nick) == false)
	{
	m_client[index].m_nick = nick;
	popNonStop(index, 1);
	std::cout << CYAN << "- Client(" << index << ") using '" << nick << "' as NICK" << NC << std::endl;
	//sendMsgClient(index, ":toxic.irc PRIVMSG " + m_client[index].m_nick + " :You are now known as " + m_client[index].m_nick);
	m_client[index].m_auth = 2;
	}
	else
	{
	std::cout << RED << "- Client(" << index << ") using a existing nickname: '" << nick << "'" << NC << std::endl;
	sendMsgClient(index, ":toxic.irc 433 * " + nick + " :Nickname is already in use");
	}
}

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

void	Server::handlePing(int index)
{
	std::string token = m_client[index].m_buffer.front();
	sendMsgClient(index, "PONG " + token);
	std::cout << CYAN << "PONG " << token << NC << std::endl;
}

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

std::string	Server::removeBeginningChar(std::string str, char c)
{
	int i = 0;

	while (str[i])
	{
		if (str[i] != c)
			break ;
		i++;
	}
	return (&str[i]);
}

std::string	Server::Deque2String(std::deque<std::string> buffer)
{
	std::string msg;
    for (std::deque<std::string>::iterator it = buffer.begin(); it != buffer.end(); ++it)
	{
        msg += *it + " ";
	}
	return (msg);
}

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