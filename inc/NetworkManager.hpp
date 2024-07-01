#ifndef NETWORK_MANAGER_HPP
#define NETWORK_MANAGER_HPP

#include <vector>
#include <string>
#include <sys/poll.h>

class NetworkManager
{
public:
	NetworkManager();
	~NetworkManager();

	void initServer(int port);
	int acceptConnection();
	void addToPollList(int fd);
	void removeFromPollList(int fd);
	int pollEvents(int timeout);
	bool isServerFd(size_t index) const;
	int getFd(size_t index) const;
	bool canRead(size_t index) const;
	void sendData(int fd, const std::string& data);
	std::string receiveData(int fd);
	void closeConnection(int fd);
	size_t getPollFdsSize() const { return pollFds.size(); }

private:
	int serverFd;
	std::vector<pollfd> pollFds;

	void setNonBlocking(int fd);
};

#endif // NETWORK_MANAGER_HPP