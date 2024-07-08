#ifndef COMMAND_HPP
#define COMMAND_HPP

#include <string>

class IRCServer;

class Command
{
public:
	virtual ~Command() {}
	virtual void execute(IRCServer& server, int client_fd, const std::string& params) = 0;
};

class CapCommand : public Command
{
public:
	void execute(IRCServer& server, int client_fd, const std::string& params);
};

class OperCommand : public Command
{
public:
	void execute(IRCServer& server, int client_fd, const std::string& params);
};

class PassCommand : public Command
{
public:
	void execute(IRCServer& server, int client_fd, const std::string& params);
};

class NickCommand : public Command
{
public:
	void execute(IRCServer& server, int client_fd, const std::string& params);
};

class UserCommand : public Command
{
public:
	void execute(IRCServer& server, int client_fd, const std::string& params);
};

class JoinCommand : public Command
{
public:
	void execute(IRCServer& server, int client_fd, const std::string& params);
};

class PrivmsgCommand : public Command
{
public:
	void execute(IRCServer& server, int client_fd, const std::string& params);
};

class KickCommand : public Command
{
public:
	void execute(IRCServer& server, int client_fd, const std::string& params);
};

class InviteCommand : public Command
{
public:
	void execute(IRCServer& server, int client_fd, const std::string& params);
};

class TopicCommand : public Command
{
public:
	void execute(IRCServer& server, int client_fd, const std::string& params);
};

class ModeCommand : public Command
{
public:
	void execute(IRCServer& server, int client_fd, const std::string& params);
};

class PartCommand : public Command
{
public:
	void execute(IRCServer& server, int client_fd, const std::string& params);
};

class QuitCommand : public Command
{
public:
	void execute(IRCServer& server, int client_fd, const std::string& params);
};

#endif // COMMAND_HPP