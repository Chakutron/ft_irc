#include "../inc/NetworkManager.hpp"
#include "../inc/IRCException.hpp"
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <unistd.h>
#include <cstring>
#include <cerrno>

NetworkManager::NetworkManager() : serverFd(-1) {}

NetworkManager::~NetworkManager()
{
	if (serverFd != -1) {
		close(serverFd);
	}
}

void NetworkManager::initServer(int port)
{
	serverFd = socket(AF_INET, SOCK_STREAM, 0);
	if (serverFd == -1)
	{
		throw IRCException("Failed to create socket: " + std::string(strerror(errno)));
	}

	int opt = 1;
	if (setsockopt(serverFd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1)
	{
		throw IRCException("Failed to set socket options: " + std::string(strerror(errno)));
	}

	sockaddr_in address;
	address.sin_family = AF_INET;
	address.sin_addr.s_addr = INADDR_ANY;
	address.sin_port = htons(port);

	if (bind(serverFd, (struct sockaddr*)&address, sizeof(address)) == -1)
	{
		throw IRCException("Failed to bind to port: " + std::string(strerror(errno)));
	}

	if (listen(serverFd, SOMAXCONN) == -1)
	{
		throw IRCException("Failed to listen on socket: " + std::string(strerror(errno)));
	}

	setNonBlocking(serverFd);
	addToPollList(serverFd);
}

int NetworkManager::acceptConnection()
{
	sockaddr_in clientAddr;
	socklen_t clientAddrLen = sizeof(clientAddr);
	int clientFd = accept(serverFd, (struct sockaddr*)&clientAddr, &clientAddrLen);
	
	if (clientFd == -1)
	{
		throw IRCException("Failed to accept new connection: " + std::string(strerror(errno)));
	}

	setNonBlocking(clientFd);
	addToPollList(clientFd);

	return clientFd;
}

void NetworkManager::addToPollList(int fd)
{
	pollfd newFd = {fd, POLLIN, 0};
	pollFds.push_back(newFd);
}

void NetworkManager::removeFromPollList(int fd)
{
	for (std::vector<pollfd>::iterator it = pollFds.begin(); it != pollFds.end(); ++it)
	{
		if (it->fd == fd)
		{
			pollFds.erase(it);
			break;
		}
	}
}

int NetworkManager::pollEvents(int timeout)
{
	int result = poll(&pollFds[0], pollFds.size(), timeout);
	if (result == -1)
	{
		throw IRCException("Poll failed: " + std::string(strerror(errno)));
	}
	return result;
}

bool NetworkManager::isServerFd(size_t index) const
{
	return pollFds[index].fd == serverFd;
}

int NetworkManager::getFd(size_t index) const
{
	return pollFds[index].fd;
}

bool NetworkManager::canRead(size_t index) const
{
	return pollFds[index].revents & POLLIN;
}

void NetworkManager::sendData(int fd, const std::string& data)
{
	ssize_t bytesSent = send(fd, data.c_str(), data.length(), 0);
	if (bytesSent == -1)
	{
		throw IRCException("Failed to send data: " + std::string(strerror(errno)));
	}
	if (static_cast<size_t>(bytesSent) < data.length())
	{
		throw IRCException("Incomplete send");
	}
}

std::string NetworkManager::receiveData(int fd)
{
	char buffer[1024];
	ssize_t bytesReceived = recv(fd, buffer, sizeof(buffer) - 1, 0);
	
	if (bytesReceived == -1)
	{
		if (errno == EWOULDBLOCK || errno == EAGAIN)
		{
			return ""; // No data available
		}
		throw IRCException("Failed to receive data: " + std::string(strerror(errno)));
	}
	
	if (bytesReceived == 0)
	{
		throw IRCException("Client disconnected");
	}
	
	buffer[bytesReceived] = '\0';
	return std::string(buffer);
}

void NetworkManager::closeConnection(int fd)
{
	close(fd);
	removeFromPollList(fd);
}

void NetworkManager::setNonBlocking(int fd)
{
	int flags = fcntl(fd, F_GETFL, 0);
	if (flags == -1)
	{
		throw IRCException("Failed to get socket flags: " + std::string(strerror(errno)));
	}
	
	if (fcntl(fd, F_SETFL, flags | O_NONBLOCK) == -1)
	{
		throw IRCException("Failed to set socket to non-blocking: " + std::string(strerror(errno)));
	}
}