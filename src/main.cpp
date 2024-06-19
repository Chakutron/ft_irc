#include <stdexcept>
#include <iostream>
#include <cstdlib>
#include "../inc/Server.hpp"

int main(int argc, char **argv)
{
  if (argc == 3)
  {
    try
    {
      Server server(std::atoi(argv[1]), argv[2]);
      server.runServ(std::atoi(argv[1]));
    }
    catch (const std::exception& e)
    {
      std::cerr << "Error: " << e.what() << std::endl;
    }
  }
  else
    std::cerr << "Usage: ./ircserv <port> <password>" << std::endl;
  return (0);
}
