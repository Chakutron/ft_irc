#include "../inc/Command.hpp"
#include "../inc/IRCServer.hpp"
#include "../inc/Logger.hpp"
#include "../inc/IRCException.hpp"

void CapCommand::execute(IRCServer& server, int client_fd, const std::string& params)
{
	LOG_DEBUG("Executing CAP command");
	server.handleCapCommand(client_fd, params);
}

void OperCommand::execute(IRCServer& server, int client_fd, const std::string& params)
{
	LOG_DEBUG("Executing OPER command");
	server.handleOperCommand(client_fd, params);
}

void PassCommand::execute(IRCServer& server, int client_fd, const std::string& params)
{
	LOG_DEBUG("Executing PASS command");
	server.handlePassCommand(client_fd, params);
}

void NickCommand::execute(IRCServer& server, int client_fd, const std::string& params)
{
	LOG_DEBUG("Executing NICK command");
	server.handleNickCommand(client_fd, params);
}

void UserCommand::execute(IRCServer& server, int client_fd, const std::string& params)
{
	LOG_DEBUG("Executing USER command");
	server.handleUserCommand(client_fd, params);
}

void JoinCommand::execute(IRCServer& server, int client_fd, const std::string& params)
{
	LOG_DEBUG("Executing JOIN command");
	server.handleJoinCommand(client_fd, params);
}

void PrivmsgCommand::execute(IRCServer& server, int client_fd, const std::string& params) {
	LOG_DEBUG("Executing PRIVMSG command");
	server.handlePrivmsgCommand(client_fd, params);
}

void KickCommand::execute(IRCServer& server, int client_fd, const std::string& params)
{
	LOG_DEBUG("Executing KICK command");
	server.handleKickCommand(client_fd, params);
}

void InviteCommand::execute(IRCServer& server, int client_fd, const std::string& params) {
	LOG_DEBUG("Executing INVITE command");
	server.handleInviteCommand(client_fd, params);
}

void TopicCommand::execute(IRCServer& server, int client_fd, const std::string& params)
{
	LOG_DEBUG("Executing TOPIC command");
	server.handleTopicCommand(client_fd, params);
}

void ModeCommand::execute(IRCServer& server, int client_fd, const std::string& params)
{
	LOG_DEBUG("Executing MODE command");
	server.handleModeCommand(client_fd, params);
}

void PartCommand::execute(IRCServer& server, int client_fd, const std::string& params)
{
	LOG_DEBUG("Executing PART command");
	server.handlePartCommand(client_fd, params);
}