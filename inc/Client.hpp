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
#include <deque>

class Client
{
  private:
    Client(const Client& other);
    Client& operator=(const Client& other);
        
  public:
    Client() {m_auth = 0;}
    ~Client() {}
    std::string m_user;
    std::string m_name;
    std::string m_nick;
    std::string m_ip;
    int m_fd;
    std::string m_tmpBuffer;
    std::deque<std::string> m_buffer;
    int m_auth;
};
