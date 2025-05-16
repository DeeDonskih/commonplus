/**
 * @file tcpserver.hpp
 * @author Dmitrii Donskikh (deedonskihdev@gmail.com)
 * @brief Simple TCP server implementation for network communications
 * @brief Provides multi-client TCP socket operations with thread pool support and request handling functionality
 * @version 0.1
 * @date 2017-11-23
 *
 * @copyright Copyright (c) 2017-2024 Dmitrii Donskikh
 *            Licensed under MIT License (https://opensource.org/licenses/MIT)
 */

#ifndef UFW_SIMPLETCPSERVER_HPP
#define UFW_SIMPLETCPSERVER_HPP

#include "ihandler.hpp"

#include <functional>
#include <iostream>
#include <mutex>
#include <string>
#include <sys/socket.h>
#include <thread>
#include <unistd.h>
#include <unordered_set>
#include <vector>

class TcpServer
{
public:
  enum TcpServerResult
  {
    Success = 0,
    SocketCreateError = 1,
    BindError = 2,
    ListenError = 3,
    AcceptError = 4,
    SetSockOptError = 5,
    ReceiveError = 6,
    SendError = 7,
    AlreadyRunning = 8,
    NotRunning = 9,
    InvalidPort = 10
  };

  using RqHandler = std::function<std::string(int, const std::string&)>;

  TcpServer(RqHandler callback): m_callback(callback), m_running(false) {}

  TcpServer(IHandler* handler): m_running(false)
  {
    m_callback = [handler](int socket, const std::string& input) {
      return handler->handle(socket, input);
    };
  }

  ~TcpServer()
  {
    stop();
  }

  bool start(int port);
  void stop();

  [[nodiscard]]
  bool isRunning() const;

private:
  int m_port{-1};
  int m_server_fd{-1};
  RqHandler m_callback;
  std::unordered_set<int> m_clients;
  std::mutex m_clients_mutex;
  std::thread m_server_thread;
  bool m_running{false};

  void run();
  void handle_client(int client_fd);
  void close_server();
};

#endif  // UFW_SIMPLETCPSERVER_HPP