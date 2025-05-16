/**
* @file tcpserver.cpp
 * @copyright Copyright (c) 2017-2024 Dmitrii Donskikh
 * Licensed under MIT License
 */

#include "tcpserver.hpp"

#include "threadpool.hpp"

#include <cstring>
#include <netinet/in.h>
#include <netinet/tcp.h>

bool TcpServer::start(int port)
{
    if (m_running) {
      return true;  // it's already running
  }
  m_port = port;
  m_running = true;

  // Create server socket
  m_server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (m_server_fd < 0) {
      std::cerr << "Failed to create socket. Errno: " << strerror(errno) << std::endl;
      return false;
  }

  int enable = 1;
    if (setsockopt(m_server_fd, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(enable)) < 0) {
      std::cerr << "Failed to set socket option SO_REUSEADDR. Errno: " << strerror(errno) << std::endl;
      close_server();
      return false;
  }

  sockaddr_in server_addr{};
  server_addr.sin_family = AF_INET;
  server_addr.sin_addr.s_addr = INADDR_ANY;
  server_addr.sin_port = htons(m_port);

    if (bind(m_server_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
      std::cerr << "Bind failed. Errno: " << strerror(errno) << std::endl;
      close_server();
      return false;
  }

    if (listen(m_server_fd, 15) < 0) {
      std::cerr << "Listen failed. Errno: " << strerror(errno) << std::endl;
      close_server();
      return false;
  }
  m_server_thread = std::thread(&TcpServer::run, this);
  return true;
}

void TcpServer::stop()
{
  std::cout << "Stopping server" << std::endl;
  m_running = false;
    // Closing listening socket
    if (m_server_fd != -1) {
      shutdown(m_server_fd, SHUT_RDWR);
      close_server();
  }
  // Closing all client sockets
  {
    std::lock_guard<std::mutex> lock(m_clients_mutex);
      for (int client_fd: m_clients) {
        shutdown(client_fd, SHUT_RDWR);
        close(client_fd);
      }
    m_clients.clear();
  }
    // Waiting server thread to complete
    if (m_server_thread.joinable()) {
      m_server_thread.join();
  }
  std::cout << "Server stopped" << std::endl;
}

bool TcpServer::isRunning() const
{
  return m_running;
}

void TcpServer::run()
{
  utils::ThreadPool client_pool(30);
    while (m_running) {
      int client_fd = accept(m_server_fd, nullptr, nullptr);
        if (client_fd >= 0) {
          std::lock_guard<std::mutex> lock(m_clients_mutex);
          m_clients.insert(client_fd);
          client_pool.enqueue(&TcpServer::handle_client, this, client_fd);
          // std::thread client_thread (&TcpServer::handle_client, this, client_fd);
          // client_thread.detach ();
        } else {
          auto error = errno;
          std::cout << "Failed to accept connection. Errno: " << strerror(error) << std::endl;
            if ((error == EAGAIN) || (error == EWOULDBLOCK) || (error == EINTR) || (error == ECONNABORTED)) {
              // TODO: add errno handling
              continue;
            } else {
              // TODO: add errno handling
              std::cout << "Fatal accepting error: " << strerror(error) << ". Server should be stopped!" << std::endl;
              break;
            }
        }
    }
  close_server();
  std::cout << "Server thread stopped" << std::endl;
}

void TcpServer::handle_client(int client_fd)
{
  std::cout << "Client connected. sockfd = " << client_fd << std::endl;
  char buffer[10 * 1024];

  {
    int enable = 1;
      if (setsockopt(client_fd, IPPROTO_TCP, TCP_NODELAY, &enable, sizeof(enable)) < 0) {
        std::cout << "Failed to set TCP_NODELAY" << std::endl;
    }
  }

    while (m_running) {
      auto bytes_read = recv(client_fd, buffer, sizeof(buffer) - 1, 0);
        if (bytes_read <= 0) {
            if (bytes_read == 0) {
              std::cout << "Client disconnected. sockfd = " << client_fd << " result = " << bytes_read << std::endl;
              break;
          }
          auto error = errno;
            if (error == EAGAIN || error == EWOULDBLOCK || error == EINTR) {
              std::cout << "Something non-critical happened while receive. Errno: " << strerror(errno) << std::endl;
              continue;
            } else {
              std::cout << "Fatal receive error. Closing connection. Reason: " << strerror(error) << std::endl;
              break;
            }
      }
      buffer[bytes_read] = '\0';
      std::string received(buffer);
      std::string response = m_callback(client_fd, received);
      std::cout << "Response: " << response << std::endl;
      auto bytes_written = send(client_fd, response.c_str(), response.size(), 0);
        if (bytes_written == 0) {
          if (response.empty()) continue;  // What the fuck did you expect to see here?
          break;
        } else if (bytes_written < 0) {
          auto error = errno;
            if (error == EAGAIN || error == EWOULDBLOCK || error == EINTR) {
              std::cout << "Something non-critical happened while send. Errno: " << strerror(errno) << std::endl;
              // TODO: add errno handling
              continue;
            } else {
              std::cout << "Fatal send error. Closing connection. Reason: " << strerror(error) << std::endl;
              // TODO: add errno handling
              break;
            }
      }
    }
  std::lock_guard<std::mutex> lock(m_clients_mutex);
  m_clients.erase(client_fd);
  close(client_fd);
}

void TcpServer::close_server()
{
    if (m_server_fd >= 0) {
      close(m_server_fd);
      m_server_fd = -1;
  }
}