#include "../inc/IRCServer.hpp"
#include "../inc/Logger.hpp"
#include "../inc/IRCException.hpp"
#include "../inc/StringUtils.hpp"
#include <iostream>
#include <sstream>
#include <algorithm>
#include <typeinfo>

IRCServer::IRCServer(std::string cliPort, std::string cliPassword)
{
	int port = this->getInt(cliPort);
	password = cliPassword;
	
	this->operUsername = "admin";
	this->operPassword = "adminpass";
	this->hostname = "localhost";
	
	try
	{
		networkManager.initServer(port);
	}
	catch (const IRCException& e)
	{
		LOG_ERROR("Failed to initialize server: " + std::string(e.what()));
		throw;
	}

	commands["CAP"] = new CapCommand();
	commands["PASS"] = new PassCommand();
	commands["NICK"] = new NickCommand();
	commands["USER"] = new UserCommand();
	commands["JOIN"] = new JoinCommand();
	commands["PRIVMSG"] = new PrivmsgCommand();
	commands["KICK"] = new KickCommand();
	commands["INVITE"] = new InviteCommand();
	commands["TOPIC"] = new TopicCommand();
	commands["MODE"] = new ModeCommand();
	commands["OPER"] = new OperCommand();
	commands["PART"] = new PartCommand();
	commands["QUIT"] = new QuitCommand();
}

int IRCServer::getInt(const std::string value) const
{
	if (!value.empty())
	{
		try
		{
			return std::atoi(value.c_str());
		}
		catch (const std::exception& e)
		{
			LOG_WARNING("Invalid integer value for key: " + value);
		}
	}
	return -1;
}

IRCServer::~IRCServer()
{
	for (std::map<int, Client*>::iterator it = clients.begin(); it != clients.end(); ++it)
	{
		delete it->second;
	}
	for (std::map<std::string, Channel*>::iterator it = channels.begin(); it != channels.end(); ++it)
	{
		delete it->second;
	}
	for (std::map<std::string, Command*>::iterator it = commands.begin(); it != commands.end(); it++)
	{
		delete it->second;
	}
}

void IRCServer::run(std::string cliPort)
{
	LOG_INFO("Server started on port " + cliPort);
	while (true)
	{
		try
		{
			int pollCount = networkManager.pollEvents(-1);
			if (pollCount == 0) continue;
			for (size_t i = 0; i < networkManager.getPollFdsSize(); ++i)
			{
				if (networkManager.canRead(i))
				{
					if (networkManager.isServerFd(i))
					{
						int newClientFd = networkManager.acceptConnection();
						clients[newClientFd] = new Client(newClientFd);
						LOG_INFO("New client connected: " + StringUtils::toString(newClientFd - 3));
					}
					else
					{
						handleClientInput(networkManager.getFd(i));
					}
				}
			}
		}
		catch (const IRCException& e)
		{
			LOG_ERROR("Error in main loop: " + std::string(e.what()));
		}
	}
}


void IRCServer::handleOperCommand(int clientFd, const std::string& params)
{
	Client* client = clients[clientFd];
	if (!client->isRegistered)
	{
		sendNumericReply(clientFd, IRCCodes::ERR_NOTREGISTERED, "*", "You have not registered");
		return;
	}
	std::istringstream iss(params);
	std::string username, password;
	iss >> username >> password;
	if (username.empty() || password.empty())
	{
		sendNumericReply(clientFd, IRCCodes::ERR_NEEDMOREPARAMS, client->nickname, "OPER :Not enough parameters");
		return;
	}

//FOR DEBUG
	// LOG_INFO("Received username: '" + username + "'");
	// LOG_INFO("Received username length: " + StringUtils::toString(username.length()));
	// LOG_INFO("Stored trimmedOpeUser: '" + trimmedOpeUser + "'");
	// LOG_INFO("Stored trimmedOpeUser length: " + StringUtils::toString(trimmedOpeUser.length()));

	// LOG_INFO("Received password: '" + password + "'");
	// LOG_INFO("Received password length: " + StringUtils::toString(password.length()));
	// LOG_INFO("Stored trimmedOpePass: '" + trimmedOpePass + "'");
	// LOG_INFO("Stored trimmedOpePass length: " + StringUtils::toString(trimmedOpePass.length()));
 
	if (username == operUsername && password == operPassword)
	{
		client->isOperator = true;
		sendNumericReply(clientFd, IRCCodes::RPL_YOUREOPER, client->nickname, ":You are now an IRC operator");
		LOG_INFO("Client " + StringUtils::toString(clientFd - 3) + " (" + client->nickname + ") is now an operator");
	}
	else
	{
		sendNumericReply(clientFd, IRCCodes::ERR_PASSWDMISMATCH, client->nickname, ":Password incorrect");
		LOG_WARNING("Failed OPER attempt by client " + StringUtils::toString(clientFd - 3) + " (" + client->nickname + ")");
	}
}


void IRCServer::sendToClient(int clientFd, const std::string& message)
{
	std::string cleanedMessage = removeConsecutiveDuplicates(message, ':');
	try
	{
		networkManager.sendData(clientFd, cleanedMessage);
	}
	catch (const IRCException& e)
	{
		LOG_ERROR("Failed to send message to client " + StringUtils::toString(clientFd - 3) + ": " + e.what());
		disconnectClient(clientFd);
	}
}

void IRCServer::sendToChannel(const Channel* channel, const std::string& message, int excludeFd)
{
	std::string cleanedMessage = removeConsecutiveDuplicates(message, ':');
	for (std::vector<Client*>::const_iterator it = channel->clients.begin(); it != channel->clients.end(); ++it)
	{
		if ((*it)->fd != excludeFd)
		{
			sendToClient((*it)->fd, cleanedMessage);
		}
	}
}

void IRCServer::sendNumericReply(int clientFd, const std::string& code, const std::string& target, const std::string& params)
{
	std::ostringstream oss;
	// oss.str("");
	oss << ":ToxicIRC " << code << " " << target << " ";
	oss << IRCCodes::getCodeMessage(code);
	if (!params.empty())
	{
		oss << " " << params;
	}
	oss << "\n";
	// LOG_INFO("Send numeric reply " + oss.str());
	// sendToClient(clientFd, oss.str());
	// oss.str("");

	std::string cleanedMessage = removeConsecutiveDuplicates(oss.str(), ':');
	sendToClient(clientFd, cleanedMessage);
}

void IRCServer::disconnectClient(int clientFd)
{
	LOG_INFO("Client disconnected: " + StringUtils::toString(clientFd - 3));
	networkManager.closeConnection(clientFd);
	delete clients[clientFd];
	clients.erase(clientFd);
	inputBuffers.erase(clientFd);
}

void IRCServer::processMessage(int clientFd, const std::string& message)
{
	std::istringstream iss(message);
	std::string command;
	iss >> command;
	LOG_INFO("Received command: " + command);
	std::transform(command.begin(), command.end(), command.begin(), ::toupper);

	std::string params;
	std::getline(iss >> std::ws, params);
	Client* client = clients[clientFd];
	if (!client->isRegistered)
	{
		handleUnregisteredCommand(clientFd, command, params);
	}
	else
	{
		std::map<std::string, Command*>::iterator it = commands.find(command);
		if (it != commands.end())
		{
			try
			{
				it->second->execute(*this, clientFd, params);
			}
			catch (const IRCException& e)
			{
				LOG_ERROR("Error executing command " + command + ": " + e.what());
				sendNumericReply(clientFd, IRCCodes::ERR_UNKNOWNERROR, "*", ":Command execution failed");
			}
		}
		else
		{
			LOG_WARNING("Unknown command received: " + command);
			sendNumericReply(clientFd, IRCCodes::ERR_UNKNOWNCOMMAND, "*", command + " :Unknown command");
		}
	}
}



void IRCServer::handleUnregisteredCommand(int clientFd, const std::string& command, const std::string& params)
{
	if (command == "CAP") {
		handleCapCommand(clientFd, params);
	} else if (command == "PASS") {
		handlePassCommand(clientFd, params);
	} else if (command == "NICK") {
		handleNickCommand(clientFd, params);
	} else if (command == "USER") {
		handleUserCommand(clientFd, params);
	} else {
		// Only send ERR_NOTREGISTERED for commands that require registration
		sendNumericReply(clientFd, IRCCodes::ERR_NOTREGISTERED, "*", command + " :You have not registered");
	}
}

void IRCServer::handleCapCommand(int clientFd, const std::string& capSubcommand)
{
	std::istringstream iss(capSubcommand);
	std::string subcommand;
	iss >> subcommand;
	std::transform(subcommand.begin(), subcommand.end(), subcommand.begin(), ::toupper);
	std::string response = ":server CAP * LS :";
	if (subcommand == "LS")
	{
		for (std::map<std::string, Command*>::iterator it = commands.begin(); it != commands.end(); it++)
		{
			response += it->first + " ";

		}
		sendToClient(clientFd, response + "\n");
		
	} else if (subcommand == "LIST")
	{
		sendToClient(clientFd, ":server CAP * LIST :\n");
	// } else if (subcommand == "REQ")
	//{
	//     sendToClient(clientFd, ":server CAP * NAK :\n");
	}
	else if (subcommand == "END")
	{
		// Do nothing, client is ending capability negotiation
	} else {
		sendNumericReply(clientFd, IRCCodes::ERR_INVALIDCAPCMD, "*", subcommand + " :Invalid CAP subcommand");
	}
}

void IRCServer::handlePassCommand(int clientFd, const std::string& pass)
{
	Client* client = clients[clientFd];
	if (client->authenticated)
	{
		sendNumericReply(clientFd, IRCCodes::ERR_ALREADYREGISTERED, "*", ":You may not reregister");
		return;
	}
	std::string trimmedPassword = pass;
	if (pass[pass.length() - 1] == '\r')
	{
		// Remove the trailing \r character
		trimmedPassword = pass.substr(0, pass.length() - 1);
	}
	if (pass[0] == ':')
	{
		// Remove the : character
		trimmedPassword = pass.substr(1, pass.length() - 1);
	}

	// FOR DEBUG
	// std::string trimmedPassword = pass.substr(0, pass.length() - 1); 
	// LOG_INFO("Received password type: " + std::string(typeid(pass).name()));
	// LOG_INFO("Received pass: '" + pass + "'");
	// LOG_INFO("Received pass length: " + StringUtils::toString(pass.length()));
	// LOG_INFO("Received password: '" + password + "'");
	// LOG_INFO("Received password length: " + StringUtils::toString(password.length()));

	// LOG_INFO("Stored password type: " + std::string(typeid(trimmedPassword).name()));
	// LOG_INFO("Stored trimmed password: '" + trimmedPassword + "'");
	// LOG_INFO("Stored trimmed password length: " + StringUtils::toString(trimmedPassword.length()));
	if (trimmedPassword == password)
	{
		client->authenticated = true;
		LOG_INFO("Client " + StringUtils::toString(clientFd - 3) + " authenticated");
	}
	else
	{
		sendNumericReply(clientFd, IRCCodes::ERR_PASSWDMISMATCH, "*", "Server password incorrect");
		// Don't disconnect here, allow the client to try again
	}
}


void IRCServer::handleNickCommand(int clientFd, const std::string& nickname)
{
	Client* client = clients[clientFd];
	if (!client->authenticated)
	{
		sendNumericReply(clientFd, IRCCodes::ERR_PASSWDMISMATCH, "*", ":Server password needed before nickname");
		return;
	}
	if (nickname.empty())
	{
		sendNumericReply(clientFd, IRCCodes::ERR_NONICKNAMEGIVEN, "*", ":No nickname given");
		return;
	}
	if (!isNicknameAvailable(nickname))
	{
		sendNumericReply(clientFd, IRCCodes::ERR_NICKNAMEINUSE, "*", nickname + " :Nickname is already in use");
		return;
	}
	std::string oldNick = client->nickname;
	client->nickname = nickname;
	if (!client->isRegistered)
	{
		if (!client->username.empty())
		{
			client->isRegistered = true;
			sendNumericReply(clientFd, IRCCodes::RPL_WELCOME, nickname, "*** Welcome to the TOXIC IRC 1.0 Network *** " + nickname + "!" + client->username + "@127.0.0.1");
		}
	}
	else
	{
		sendToClient(clientFd, ":" + oldNick + " NICK :" + nickname + "\n");
	}
}

void IRCServer::handleUserCommand(int clientFd, const std::string& userInfo)
{
	Client* client = clients[clientFd];
	if (!client->authenticated)
	{
		sendNumericReply(clientFd, IRCCodes::ERR_PASSWDMISMATCH, "*", ":Server password needed before nickname");
		return;
	}
	if (client->isRegistered)
	{
		sendNumericReply(clientFd, IRCCodes::ERR_ALREADYREGISTERED, "*", ":You may not reregister");
		return;
	}
	std::istringstream iss(userInfo);
	std::string username, hostname, servername, realname;
	iss >> username >> hostname >> servername;
	std::getline(iss >> std::ws, realname);
	if (username.empty() || hostname.empty() || servername.empty() || realname.empty())
	{
		sendNumericReply(clientFd, IRCCodes::ERR_NEEDMOREPARAMS, "*", "USER :Not enough parameters");
		return;
	}
	client->username = username;
	client->realname = realname;
	if (!client->nickname.empty())
	{
		client->isRegistered = true;
		sendNumericReply(clientFd, IRCCodes::RPL_WELCOME, client->nickname, "*** Welcome to the TOXIC IRC 1.0 Network *** " + client->nickname + "!" + client->username + "@127.0.0.1");
	}
}

void IRCServer::handleJoinCommand(int clientFd, const std::string& channelInfo)
{
	Client* client = clients[clientFd];
	if (!client->isRegistered)
	{
		sendNumericReply(clientFd, IRCCodes::ERR_NOTREGISTERED, "*", ":You have not registered");
		return;
	}

	std::istringstream iss(channelInfo);
	std::string channelName;
	iss >> channelName;

	if (channelName.empty() || channelName[0] != '#')
	{
		sendNumericReply(clientFd, IRCCodes::ERR_NEEDMOREPARAMS, client->nickname, "JOIN :Not enough parameters");
		return;
	}

	Channel* channel = getOrCreateChannel(channelName);
	
	if (channel->invite_only && std::find(channel->clients.begin(), channel->clients.end(), client) == channel->clients.end())
	{
		sendNumericReply(clientFd, IRCCodes::ERR_INVITEONLYCHAN, client->nickname, channelName + " :Cannot join channel (+i)");
		return;
	}

	if (channel->clients.size() >= static_cast<size_t>(channel->user_limit) && channel->user_limit > 0)
	{
		sendNumericReply(clientFd, IRCCodes::ERR_CHANNELISFULL, client->nickname, channelName + " :Cannot join channel (+l)");
		return;
	}

	channel->addClient(client);
	if (channel->clients.size() == 1)
		channel->addOperator(client);
	sendToChannel(channel, ":" + client->nickname + " JOIN " + channelName + "\n");
	
	if (!channel->topic.empty())
	{
		sendNumericReply(clientFd, IRCCodes::RPL_TOPIC, client->nickname, channelName + " :" + channel->topic);
	}
	
	std::string userList;
	for (std::vector<Client*>::const_iterator it = channel->clients.begin(); it != channel->clients.end(); ++it)
	{
		if (!userList.empty())
		{
			userList += " ";
		}
		if (channel->isOperator(*it))
			userList += "@" + (*it)->nickname;
		else
			userList += (*it)->nickname;
	}
	sendNumericReply(clientFd, IRCCodes::RPL_NAMREPLY, client->nickname, "= " + channelName + " :" + userList);
	sendNumericReply(clientFd, IRCCodes::RPL_ENDOFNAMES, client->nickname, channelName + " :End of /NAMES list");
}

void IRCServer::handlePrivmsgCommand(int clientFd, const std::string& messageInfo)
{
	Client* sender = clients[clientFd];
	if (!sender->isRegistered)
	{
		sendNumericReply(clientFd, IRCCodes::ERR_NOTREGISTERED, "*", ":You have not registered");
		return;
	}

	std::istringstream iss(messageInfo);
	std::string target, message;
	iss >> target;
	std::getline(iss >> std::ws, message);

	if (target.empty() || message.empty())
	{
		sendNumericReply(clientFd, IRCCodes::ERR_NEEDMOREPARAMS, sender->nickname, "PRIVMSG :Not enough parameters");
		return;
	}

	if (target[0] == '#')
	{
		Channel* channel = getOrCreateChannel(target);
		if (std::find(channel->clients.begin(), channel->clients.end(), sender) == channel->clients.end())
		{
			sendNumericReply(clientFd, IRCCodes::ERR_CANNOTSENDTOCHAN, sender->nickname, target + " :Cannot send to channel");
			return;
		}
		sendToChannel(channel, ":" + sender->nickname + " PRIVMSG " + target + " :" + message + "\n", clientFd);
	}
	else
	{
		Client* recipient = getClientByNickname(target);
		if (recipient)
		{
			sendToClient(recipient->fd, ":" + sender->nickname + " PRIVMSG " + target + " :" + message + "\n");
		}
		else
		{
			sendNumericReply(clientFd, IRCCodes::ERR_NOSUCHNICK, sender->nickname, target + " :No such nick/channel");
		}
	}
}

void IRCServer::handleKickCommand(int clientFd, const std::string& kickInfo)
{
	Client* kicker = clients[clientFd];
	if (!kicker->isRegistered)
	{
		sendNumericReply(clientFd, IRCCodes::ERR_NOTREGISTERED, "*", ":You have not registered");
		return;
	}

	std::istringstream iss(kickInfo);
	std::string channelName, targetNick, reason;
	iss >> channelName >> targetNick;
	std::getline(iss >> std::ws, reason);

	if (channelName.empty() || targetNick.empty())
	{
		sendNumericReply(clientFd, IRCCodes::ERR_NEEDMOREPARAMS, kicker->nickname, "KICK :Not enough parameters");
		return;
	}

	Channel* channel = getOrCreateChannel(channelName);
	if (!channel->isOperator(kicker))
	{
		sendNumericReply(clientFd, IRCCodes::ERR_CHANOPRIVSNEEDED, kicker->nickname, channelName + " :You're not channel operator");
		return;
	}

	Client* target = getClientByNickname(targetNick);
	if (!target)
	{
		sendNumericReply(clientFd, IRCCodes::ERR_NOSUCHNICK, kicker->nickname, targetNick + " :No such nick/channel");
		return;
	}

	if (std::find(channel->clients.begin(), channel->clients.end(), target) == channel->clients.end()) {
		sendNumericReply(clientFd, IRCCodes::ERR_USERNOTINCHANNEL, kicker->nickname, targetNick + " " + channelName + " :They aren't on that channel");
		return;
	}

	channel->removeClient(target);
	std::string kickMessage = ":" + kicker->nickname + " KICK " + channelName + " " + targetNick + " :" + reason + "\n";
	sendToChannel(channel, kickMessage);
	sendToClient(target->fd, kickMessage);
}

void IRCServer::handleInviteCommand(int clientFd, const std::string& inviteInfo)
{
	Client* inviter = clients[clientFd];
	if (!inviter->isRegistered)
	{
		sendNumericReply(clientFd, IRCCodes::ERR_NOTREGISTERED, "*", ":You have not registered");
		return;
	}

	std::istringstream iss(inviteInfo);
	std::string targetNick, channelName;
	iss >> targetNick >> channelName;

	if (targetNick.empty() || channelName.empty())
	{
		sendNumericReply(clientFd, IRCCodes::ERR_NEEDMOREPARAMS, inviter->nickname, "INVITE :Not enough parameters");
		return;
	}

	Channel* channel = getOrCreateChannel(channelName);
	if (!channel->isOperator(inviter))
	{
		sendNumericReply(clientFd, IRCCodes::ERR_CHANOPRIVSNEEDED, inviter->nickname, channelName + " :You're not channel operator");
		return;
	}

	Client* target = getClientByNickname(targetNick);
	if (!target)
	{
		sendNumericReply(clientFd, IRCCodes::ERR_NOSUCHNICK, inviter->nickname, targetNick + " :No such nick/channel");
		return;
	}

	if (std::find(channel->clients.begin(), channel->clients.end(), target) != channel->clients.end())
	{
		sendNumericReply(clientFd, IRCCodes::ERR_USERONCHANNEL, inviter->nickname, targetNick + " " + channelName + " :is already on channel");
		return;
	}

	sendToClient(target->fd, ":" + inviter->nickname + " INVITE " + targetNick + " :" + channelName + "\n");
	sendNumericReply(clientFd, IRCCodes::RPL_INVITING, inviter->nickname, targetNick + " " + channelName);
}

void IRCServer::handleTopicCommand(int clientFd, const std::string& topicInfo)
{
	Client* client = clients[clientFd];
	if (!client->isRegistered)
	{
		sendNumericReply(clientFd, IRCCodes::ERR_NOTREGISTERED, "*", ":You have not registered");
		return;
	}

	std::istringstream iss(topicInfo);
	std::string channelName, newTopic;
	iss >> channelName;
	std::getline(iss >> std::ws, newTopic);

	if (channelName.empty())
	{
		sendNumericReply(clientFd, IRCCodes::ERR_NEEDMOREPARAMS, client->nickname, "TOPIC :Not enough parameters");
		return;
	}

	Channel* channel = getOrCreateChannel(channelName);
	if (std::find(channel->clients.begin(), channel->clients.end(), client) == channel->clients.end())
	{
		sendNumericReply(clientFd, IRCCodes::ERR_NOTONCHANNEL, client->nickname, channelName + " :You're not on that channel");
		return;
	}

	if (newTopic.empty())
	{
		if (channel->topic.empty())
		{
			sendNumericReply(clientFd, IRCCodes::RPL_NOTOPIC, client->nickname, channelName + " :No topic is set");
		}
		else
		{
			sendNumericReply(clientFd, IRCCodes::RPL_TOPIC, client->nickname, channelName + " :" + channel->topic);
		}
	}
	else
	{
		if (!channel->isOperator(client))
		{
			sendNumericReply(clientFd, IRCCodes::ERR_CHANOPRIVSNEEDED, client->nickname, channelName + " :You're not channel operator");
			return;
		}
		channel->topic = newTopic;
		sendToChannel(channel, ":" + client->nickname + " TOPIC " + channelName + " :" + newTopic + "\n");
	}
}

void IRCServer::handleModeCommand(int clientFd, const std::string& modeInfo)
{
	Client* client = clients[clientFd];
	if (!client->isRegistered) {
		sendNumericReply(clientFd, IRCCodes::ERR_NOTREGISTERED, "*", ":You have not registered");
		return;
	}

	std::istringstream iss(modeInfo);
	std::string channelName, modes, modeParams;
	iss >> channelName >> modes;
	std::getline(iss >> std::ws, modeParams);

	if (channelName.empty() || modes.empty())
	{
		sendNumericReply(clientFd, IRCCodes::ERR_NEEDMOREPARAMS, client->nickname, "MODE :Not enough parameters");
		return;
	}

	Channel* channel = getOrCreateChannel(channelName);
	if (!client->isOperator)
	{
		sendNumericReply(clientFd, IRCCodes::ERR_CHANOPRIVSNEEDED, client->nickname, channelName + " :You're not an operator");
		return;
	}

	std::string modeChanges;
	std::istringstream modeParamsStream(modeParams);
	std::string param;
	bool adding = true;

	for (size_t i = 0; i < modes.length(); ++i)
	{
		char mode = modes[i];
		switch (mode)
		{
			case '+':
				adding = true;
				break;
			case '-':
				adding = false;
				break;
			case 'i':
				channel->invite_only = adding;
				modeChanges += (adding ? "+" : "-") + std::string(1, mode);
				break;
			case 'k':
				if (adding)
				{
					if (modeParamsStream >> param)
					{
						channel->key = param;
						modeChanges += "+" + std::string(1, mode) + " " + param;
					}
				}
				else
				{
					channel->key.clear();
					modeChanges += "-" + std::string(1, mode);
				}
				break;
			case 'l':
				if (adding)
				{
					if (modeParamsStream >> param)
					{
						channel->user_limit = std::atoi(param.c_str());
						modeChanges += "+" + std::string(1, mode) + " " + param;
					}
				}
				else
				{
					channel->user_limit = 0;
					modeChanges += "-" + std::string(1, mode);
				}
				break;
			case 'o':
				if (modeParamsStream >> param)
				{
					Client* targetClient = getClientByNickname(param);
					if (targetClient)
					{
						if (adding)
						{
							channel->addOperator(targetClient);
							LOG_INFO(targetClient->nickname + " is now an operator on " + channel->name);
						}
						else
						{
							channel->removeOperator(targetClient);
						}
						modeChanges += (adding ? "+" : "-") + std::string(1, mode) + " " + param;
					}
				}
				break;
			default:
				sendNumericReply(clientFd, IRCCodes::ERR_UNKNOWNMODE, client->nickname, std::string(1, mode) + " :is unknown mode char to me");
				break;
		}
	}

	if (!modeChanges.empty())
	{
		sendToChannel(channel, ":" + client->nickname + " MODE " + channelName + " " + modeChanges + "\n");
	}
}

bool IRCServer::isNicknameAvailable(const std::string& nickname) 
{
	for (std::map<int, Client*>::const_iterator it = clients.begin(); it != clients.end(); ++it) 
	{
		if (it->second->nickname == nickname)
			return false;
	 }
	return true;
}

Client* IRCServer::getClientByNickname(const std::string& nickname)
{
	for (std::map<int, Client*>::const_iterator it = clients.begin(); it != clients.end(); ++it)
	{
		if (it->second->nickname == nickname)
		{
			return it->second;
		}
	}
	return NULL;
}

Channel* IRCServer::getOrCreateChannel(const std::string& channelName)
{
	std::map<std::string, Channel*>::iterator it = channels.find(channelName);
	if (it == channels.end())
	{
		Channel* newChannel = new Channel(channelName);
		channels[channelName] = newChannel;
		return newChannel;
	}
	return it->second;
}

void IRCServer::handleClientInput(int clientFd)
{
	try {
		std::string data = networkManager.receiveData(clientFd);
		if (data.empty())
		{
			return;  // No data received, possibly due to non-blocking socket
		}
		inputBuffers[clientFd] += data;  // Append new data to the buffer
		processBuffer(clientFd);
	}
	catch (const IRCException& e)
	{
		LOG_WARNING("Error reading from client " + StringUtils::toString(clientFd - 3) + ": " + e.what());
		disconnectClient(clientFd);
	}
}

void IRCServer::processBuffer(int clientFd)
{
	std::string& buffer = inputBuffers[clientFd];
	size_t pos;
	while ((pos = buffer.find_first_of("\r\n")) != std::string::npos)
	{
		std::string command = buffer.substr(0, pos);
		// buffer.erase(0, pos + 2);  // Remove processed command and "\n"
		// Check if the command ends with \r\n
		if (pos + 1 < buffer.length() && buffer[pos] == '\r' && buffer[pos + 1] == '\n')
		{
			buffer.erase(0, pos + 2); // Remove processed command and "\r\n"
		}
		else
		{
			buffer.erase(0, pos + 1); // Remove processed command and "\r" or "\n"
		}

		if (!command.empty())
		{
			processMessage(clientFd, command);
		}
	}
	//LOG_INFO("Buffer for client " + StringUtils::toString(clientFd - 3) + ": " + buffer);

	// handle very long lines to prevent buffer overflow
	if (buffer.length() > 512)
	{  // IRC traditionally limits lines to 512 bytes
		buffer.erase(0, buffer.length());
		LOG_WARNING("Buffer overflow from client " + StringUtils::toString(clientFd - 3) + ", clearing buffer");
	}
}

void IRCServer::handlePartCommand(int clientFd, const std::string& params)
{
	Client* client = clients[clientFd];
	if (!client->isRegistered)
	{
		sendNumericReply(clientFd, IRCCodes::ERR_NOTREGISTERED, "*", ":You have not registered");
		return;
	}

	std::istringstream iss(params);
	std::string channelName;
	iss >> channelName;

	if (channelName.empty() || channelName[0] != '#')
	{
		sendNumericReply(clientFd, IRCCodes::ERR_NEEDMOREPARAMS, client->nickname, "JOIN :Not enough parameters");
		return;
	}

	Channel* channel = getOrCreateChannel(channelName);

	channel->removeClient(client);

	std::string userList;
	for (std::vector<Client*>::const_iterator it = channel->clients.begin(); it != channel->clients.end(); ++it)
	{
		if (!userList.empty())
		{
			userList += " ";
		}
		if (channel->isOperator(*it))
			userList += "@" + (*it)->nickname;
		else
			userList += (*it)->nickname;
	}





	for (std::vector<Client*>::const_iterator it = channel->clients.begin(); it != channel->clients.end(); ++it)
	{
		//sendToChannel(channel, client->nickname + " left the channel", clientFd);
		sendNumericReply((*it)->fd, IRCCodes::RPL_NAMREPLY, client->nickname, "= " + channelName + " :" + userList);
		sendNumericReply((*it)->fd, IRCCodes::RPL_ENDOFNAMES, client->nickname, channelName + " :End of /NAMES list");
		//std::cout << "client(" << (*it)->fd << ") " << client->nickname << "!" << client->username << "@hostname PART " << channelName << " :Goodbye " << (*it)->nickname << std::endl;
		//sendToClient((*it)->fd, client->nickname + "!" + client->username + "@hostname PART " + channelName + " :Goodbye " + (*it)->nickname);
	}
}

void IRCServer::handleQuitCommand(int clientFd, const std::string& quitMessage) {
	Client* client = clients[clientFd];
	if (!client) {
		LOG_WARNING("Attempt to quit from non-existent client: " + StringUtils::toString(clientFd - 3));
		return;
	}
	std::string fullQuitMessage = ":" + client->nickname + "!" + client->username + "@" + this->hostname + " QUIT ";
	if (!quitMessage.empty()) {
		fullQuitMessage += quitMessage;
	} else {
		fullQuitMessage += "Client Quit";
	}
	// Inform all channels this client was in about the quit
	for (std::map<std::string, Channel*>::iterator it = channels.begin(); it != channels.end(); ++it) {
		Channel* channel = it->second;
		if (std::find(channel->clients.begin(), channel->clients.end(), client) != channel->clients.end()) {
			sendToChannel(channel, fullQuitMessage + "\r\n", clientFd);
			channel->removeClient(client);
		}
	}
	LOG_INFO("Client " + StringUtils::toString(clientFd - 3) + " (" + client->nickname + ") has quit ");
	disconnectClient(clientFd);
}