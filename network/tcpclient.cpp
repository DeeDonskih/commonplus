/**
* @file tcpclient.cpp
 * @copyright Copyright (c) 2017-2024 Dmitrii Donskikh
 * Licensed under MIT License
 */
#include "tcpclient.hpp"

#include <arpa/inet.h>
#include <cstring>
#include <iostream>
#include <string>
#include <sys/socket.h>
#include <unistd.h>

bool TcpClient::connect(const std::string& ip, uint16_t port)
{
  m_sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (m_sockfd == -1) {
      std::cerr << "Error creating socket\n";
      return false;
  }

  sockaddr_in server_addr{};
  server_addr.sin_family = AF_INET;
  server_addr.sin_port = htons(port);
    if (inet_pton(AF_INET, ip.c_str(), &server_addr.sin_addr) <= 0) {
      std::cerr << "Invalid address\n";
      return false;
  }

    if (::connect(m_sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
      std::cerr << "Connection failed\n";
      return false;
  }

  return true;
}

void TcpClient::disconnect()
{
    if (m_sockfd != -1) {
      close(m_sockfd);
      m_sockfd = -1;
  }
}

bool TcpClient::send(const std::string& data)
{
  return sendData(reinterpret_cast<const uint8_t*>(data.data()), data.size());
}

bool TcpClient::send(const std::vector<uint8_t>& data)
{
  return sendData(data.data(), data.size());
}

bool TcpClient::sendData(const uint8_t* data, size_t size)
{
    if (m_sockfd == -1) {
      std::cerr << "Socket is not connected\n";
      return false;
  }

  ssize_t sent = ::send(m_sockfd, data, size, 0);
    if (sent == -1) {
      std::cerr << "Failed to send data\n";
      return false;
  }
  return true;
}

std::optional<std::string> TcpClient::request(const std::string& data, int timeout_ms)
{
  std::cout << "TcpClient::request : " << data << std::endl;
    if (!send(data)) {
      return std::nullopt;
  }

  fd_set readfds;
  FD_ZERO(&readfds);
  FD_SET(m_sockfd, &readfds);

  timeval tv{};
  tv.tv_sec = timeout_ms / 1000;
  tv.tv_usec = (timeout_ms % 1000) * 1000;

  int retval = select(m_sockfd + 1, &readfds, nullptr, nullptr, &tv);
    if (retval == -1) {
      std::cerr << "Select error\n";
      return std::nullopt;
    } else if (retval == 0) {
      std::cerr << "Timeout waiting for response\n";
      return std::nullopt;
  }
  std::string response;
  char buffer[1024] = {0};
  ssize_t received = recv(m_sockfd, buffer, sizeof(buffer) - 1, 0);
    if (received == -1) {
      std::cerr << "Failed to receive data\n";
      return std::nullopt;
  }

  response.assign(buffer, received);
  std::cout << "TcpClient::response : " << data << std::endl;
  return response;
}