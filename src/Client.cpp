#include "../inc/Client.hpp"

Client::Client(int fd) : fd(fd), authenticated(false), isRegistered(false), isOperator(false) {}

Client::~Client() {}