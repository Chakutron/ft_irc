#ifndef IRCSERVER_HPP
#define IRCSERVER_HPP

#include <string>
#include <map>
#include "Client.hpp"
#include "Channel.hpp"
#include "Command.hpp"
#include "NetworkManager.hpp"
#include "IRCCodes.hpp"

class IRCServer
{
private:
	NetworkManager networkManager;
	std::string password;
	std::map<int, Client*> clients;
	std::map<std::string, Channel*> channels;
	std::map<std::string, Command*> commands;
	std::map<int, std::string> inputBuffers;
	std::string operUsername;
	std::string operPassword;
	std::string hostname;

	void handleClientInput(int clientFd);
	void processMessage(int clientFd, const std::string& message);
	void disconnectClient(int clientFd);
	bool isNicknameAvailable(const std::string& nickname);
	Client* getClientByNickname(const std::string& nickname);
	Channel* getOrCreateChannel(const std::string& channelName);
	int getInt(const std::string value) const;


public:
	IRCServer(std::string cliPort, std::string cliPassword);
	~IRCServer();
	void run(std::string cliPort);

	void processBuffer(int clientFd);

	void sendToClient(int clientFd, const std::string& message);
	void sendToChannel(const Channel* channel, const std::string& message, int excludeFd = -1);
	void sendNumericReply(int clientFd, const std::string& code, const std::string& target, const std::string& params);

	// Command handlers
	void handleCapCommand(int clientFd, const std::string& capSubcommand);
	void handlePassCommand(int clientFd, const std::string& pass);
	void handleNickCommand(int clientFd, const std::string& nickname);
	void handleUserCommand(int clientFd, const std::string& userInfo);
	void handleJoinCommand(int clientFd, const std::string& channelInfo);
	void handlePrivmsgCommand(int clientFd, const std::string& messageInfo);
	void handleKickCommand(int clientFd, const std::string& kickInfo);
	void handleInviteCommand(int clientFd, const std::string& inviteInfo);
	void handleTopicCommand(int clientFd, const std::string& topicInfo);
	void handleModeCommand(int clientFd, const std::string& modeInfo);
	void handleUnregisteredCommand(int clientFd, const std::string& command, const std::string& params);
	void handleOperCommand(int clientFd, const std::string& params);
	void handlePartCommand(int clientFd, const std::string& params);
	void handleQuitCommand(int clientFd, const std::string& quitMessage);

};

#endif // IRCSERVER_HPP