#include "../inc/IRCCodes.hpp"
#include <map>

namespace IRCCodes
{
	static std::map<std::string, std::string> codeMessages;

	static void initCodeMessages()
	{
		// if (codeMessages.empty())
		//{
		//     codeMessages[RPL_WELCOME] = "Welcome to the Internet Relay Network";
		//     codeMessages[RPL_YOURHOST] = "Your host is %s, running version %s";
		//     codeMessages[RPL_CREATED] = "This server was created %s";
		//     codeMessages[RPL_MYINFO] = "%s %s %s %s";
		//     codeMessages[RPL_TOPIC] = "%s :%s";
		//     codeMessages[RPL_NAMREPLY] = "%s";
		//     codeMessages[RPL_ENDOFNAMES] = "%s :End of /NAMES list";

		//     codeMessages[ERR_NOSUCHNICK] = "%s :No such nick/channel";
		//     codeMessages[ERR_NOSUCHCHANNEL] = "%s :No such channel";
		//     codeMessages[ERR_CANNOTSENDTOCHAN] = "%s :Cannot send to channel";
		//     codeMessages[ERR_TOOMANYCHANNELS] = "%s :You have joined too many channels";
		//     codeMessages[ERR_NONICKNAMEGIVEN] = ":No nickname given";
		//     codeMessages[ERR_ERRONEUSNICKNAME] = "%s :Erroneous nickname";
		//     codeMessages[ERR_NICKNAMEINUSE] = "%s :Nickname is already in use";
		//     codeMessages[ERR_NOTONCHANNEL] = "%s :You're not on that channel";
		//     codeMessages[ERR_NEEDMOREPARAMS] = "%s :Not enough parameters";
		//     codeMessages[ERR_ALREADYREGISTERED] = ":You may not reregister";
		//     codeMessages[ERR_PASSWDMISMATCH] = ":Password incorrect";
		//     codeMessages[ERR_CHANNELISFULL] = "%s :Cannot join channel (+l)";
		//     codeMessages[ERR_INVITEONLYCHAN] = "%s :Cannot join channel (+i)";
		//     codeMessages[ERR_BANNEDFROMCHAN] = "%s :Cannot join channel (+b)";
		//     codeMessages[ERR_BADCHANNELKEY] = "%s :Cannot join channel (+k)";
		//     codeMessages[ERR_NOPRIVILEGES] = ":Permission Denied- You're not an IRC operator";
		//     codeMessages[ERR_CHANOPRIVSNEEDED] = "%s :You're not channel operator";
		// }
	}

	std::string getCodeMessage(const std::string& code)
	{
		(void) code;
		initCodeMessages();
		// std::map<std::string, std::string>::iterator it = codeMessages.find(code);
		// if (it != codeMessages.end())
		//{
		//     return it->second;
		// }
		// return "Unknown code";
		return "";
	}
}