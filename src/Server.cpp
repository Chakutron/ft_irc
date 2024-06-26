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