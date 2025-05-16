/**
 * @file tcpclient.hpp
 * @author Dmitrii Donskikh (deedonskihdev@gmail.com)
 * @brief Simple TCP client implementation for network communications
 * @brief Provides basic TCP socket operations like connect, send and receive data
 * @version 0.1
 * @date 2017-11-23
 *
 * @copyright Copyright (c) 2017-2024 Dmitrii Donskikh
 *            Licensed under MIT License (https://opensource.org/licenses/MIT)
 */

#ifndef UFW_SIMPLE_TCPCLIENT_HPP
#define UFW_SIMPLE_TCPCLIENT_HPP

#include <cstdint>
#include <optional>
#include <string>
#include <vector>

class TcpClient
{
public:
  TcpClient(): m_sockfd(-1) {}

  ~TcpClient()
  {
    disconnect();
  }

  bool connect(const std::string& ip, uint16_t port);
  void disconnect();
  bool send(const std::string& data);
  bool send(const std::vector<uint8_t>& data);
  std::optional<std::string> request(const std::string& data, int timeout_ms);

private:
  int m_sockfd;
  bool sendData(const uint8_t* data, size_t size);
};

#endif  // UFW_SIMPLE_TCPCLIENT_HPP