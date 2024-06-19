#pragma once

#include <deque>

class Channel
{
  private:
    Channel(const Channel& other);
    Channel& operator=(const Channel& other);
        
  public:
    Channel() {m_inviteOnly = false; m_restrictedTopic = false; m_userLimit = 0;}
    ~Channel() {}

    std::string             m_pwd;
    std::string             m_topic;
    std::deque<std::string> m_operators;
    std::deque<std::string> m_users;
    bool                    m_inviteOnly;
    bool                    m_restrictedTopic;
    int                     m_userLimit;
};