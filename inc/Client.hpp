#pragma once

#include <deque>

class Client
{
  private:
    Client(const Client& other);
    Client& operator=(const Client& other);
        
  public:
    Client() {m_auth = 0;}
    ~Client() {}

    int                     m_fd;
    std::deque<std::string> m_buffer;
    int                     m_auth;
    std::string             m_ip;
    std::string             m_nick;
    std::string             m_user;
    std::string             m_name;
    std::string             m_tmpBuffer;
};
