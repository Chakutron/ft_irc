#include "../inc/Server.hpp"

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