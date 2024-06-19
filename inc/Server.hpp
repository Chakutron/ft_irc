#pragma once

#include <iostream>
#include <sys/socket.h>
#include <cstdio>
#include <cstdlib>
#include <arpa/inet.h>
#include <string>
#include <unistd.h>
#include <poll.h>
#include <sys/ioctl.h>
#include <cerrno>
#include <cstring>
#include <crypt.h>
#include <map>
#include <sstream>
#include <functional>
#include "Client.hpp"

#define RED "\e[31m"
#define GREEN "\e[32m"
#define YELLOW "\e[33m"
#define BLUE "\e[34m"
#define VIOLET "\e[35m"
#define CYAN "\e[36m"
#define NC "\e[0m"

#define MAX_CLIENTS 42

class Server;

typedef void (Server::*CommandFunction)(int);

class Server
{
  private:
    Server();
    Server(const Server& other);
    Server& operator=(const Server& other);
        
    int m_serverFD;
    int m_port;
    std::string m_pwd;   
    struct pollfd m_fds[MAX_CLIENTS + 1];
    Client m_client[MAX_CLIENTS + 1];

    std::map<std::string, CommandFunction> m_commandsMap;
    
    void setBuffer(const char *buffer, int index);
    bool validBuffer(const char *buffer);
    void splitBuffer(int index, std::string str);
    void execBuffer(int index);

    void handlePass(int index);
    void handleNick(int index);
    void handleUser(int index);
    void handlePing(int index);
    void handleJoin(int index);
    void handleQuit(int index);
    void handleKick(int index);
    void handleInvite(int index);
    void handleTopic(int index);
    void handleMode(int index);
    
    bool checkNick(std::string nick);
    void joinChannel(int index, std::string channel);
    
    void sendMsgClient(int client, std::string msg);

    void popNonStop(int index, int num);
    std::string	removeBeginningChar(std::string str, char c);

  public:
    Server(int port, std::string pwd);
    ~Server();

    void runServ(int port);
};
