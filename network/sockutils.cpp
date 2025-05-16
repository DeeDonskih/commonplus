
#include "sockutils.hpp"

#include <arpa/inet.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <optional>
#include <string>
#include <sys/socket.h>
#include <sys/stat.h>
#include <unistd.h>
#include <utility>


namespace ucommon
{
  bool IsFdOpen(int fd)
  {
    struct stat st;
    return fstat(fd, &st) == 0;
  }

  bool IsSocket(int fd)
  {
    struct stat st;
    return (fstat(fd, &st) == 0) && S_ISSOCK(st.st_mode);
  }

  std::optional<std::pair<SocketFamily, SocketType>> GetSocketType(int fd)
  {
    int type;
    socklen_t type_len = sizeof(type);
    if (getsockopt(fd, SOL_SOCKET, SO_TYPE, &type, &type_len) == -1) return std::nullopt;

    sockaddr_storage addr;
    socklen_t addr_len = sizeof(addr);
    if (getsockname(fd, (struct sockaddr*)&addr, &addr_len) == -1) return std::nullopt;

    SocketFamily family;
      switch (addr.ss_family) {
        case AF_INET: family = SocketFamily::IPv4; break;
        case AF_INET6: family = SocketFamily::IPv6; break;
        default: family = SocketFamily::Unknown;
      }

    SocketType sock_type;
      switch (type) {
        case SOCK_STREAM: sock_type = SocketType::Stream; break;
        case SOCK_DGRAM: sock_type = SocketType::Datagram; break;
        case SOCK_RAW: sock_type = SocketType::Raw; break;
        default: sock_type = SocketType::Unknown;
      }

    return std::make_pair(family, sock_type);
  }

  bool IsTcpConnected(int fd)
  {
    struct tcp_info info;
    socklen_t len = sizeof(info);
    if (getsockopt(fd, IPPROTO_TCP, TCP_INFO, &info, &len) == -1) return false;
    return info.tcpi_state == TCP_ESTABLISHED;
  }

  std::optional<std::pair<std::string, int>> GetPeerAddress(int fd)
  {
    struct sockaddr_storage addr;
    socklen_t addr_len = sizeof(addr);
    if (getpeername(fd, (struct sockaddr*)&addr, &addr_len) == -1) return std::nullopt;

    char host[INET6_ADDRSTRLEN];
    int port;

      if (addr.ss_family == AF_INET) {
        auto* s = (struct sockaddr_in*)&addr;
        inet_ntop(AF_INET, &s->sin_addr, host, sizeof(host));
        port = ntohs(s->sin_port);
      } else if (addr.ss_family == AF_INET6) {
        auto* s = (struct sockaddr_in6*)&addr;
        inet_ntop(AF_INET6, &s->sin6_addr, host, sizeof(host));
        port = ntohs(s->sin6_port);
      } else {
        return std::nullopt;
      }

    return std::make_pair(std::string(host), port);
  }

  std::optional<std::pair<std::string, int>> GetLocalAddress(int fd)
  {
    struct sockaddr_storage addr;
    socklen_t addr_len = sizeof(addr);
    if (getsockname(fd, (struct sockaddr*)&addr, &addr_len) == -1) return std::nullopt;

    char host[INET6_ADDRSTRLEN];
    int port;

      if (addr.ss_family == AF_INET) {
        auto* s = (struct sockaddr_in*)&addr;
        inet_ntop(AF_INET, &s->sin_addr, host, sizeof(host));
        port = ntohs(s->sin_port);
      } else if (addr.ss_family == AF_INET6) {
        auto* s = (struct sockaddr_in6*)&addr;
        inet_ntop(AF_INET6, &s->sin6_addr, host, sizeof(host));
        port = ntohs(s->sin6_port);
      } else {
        return std::nullopt;
      }

    return std::make_pair(std::string(host), port);
  }

}  // namespace ucommon
