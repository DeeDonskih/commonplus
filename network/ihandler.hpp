
#ifndef UFW_IHANDLER_HPP
#define UFW_IHANDLER_HPP

#include <string>

class IHandler
{
public:
  virtual ~IHandler() = default;
  virtual std::string handle(int socket, const std::string& input) = 0;
};

#endif  // UFW_IHANDLER_HPP