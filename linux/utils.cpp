
/**
 * @file utils.cpp
 * @author Dmitrii Donskikh (deedonskihdev@gmail.com)
 * @copyright Copyright (c) 2019-2024 Dmitrii Donskikh
 *            Licensed under the MIT License (https://opensource.org/licenses/MIT)
 */

#include "utils.hpp"

#include <cerrno>
#include <cstring>
#include <ctime>
#include <fcntl.h>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>

#define MS_TO_TIMEVAL(ms_time, tv_struct)                                                                              \
    do {                                                                                                               \
      tv_struct.tv_sec = (ms_time) / 1000;                                                                             \
      tv_struct.tv_usec = ((ms_time) % 1000) * 1000;                                                                   \
  } while (0)

namespace ucommon
{
  int untar(const std::string& filename, const std::string& to) noexcept
  {
    std::ostringstream command;
    command << "tar -xf " << '"' << filename << '"' << " -C " << '"' << to << '"';
    return std::system(command.str().c_str());
  }

  int recvFile(size_t size, int socket, const std::string& file, uint32_t timeout_ms)
  {
    const size_t BUFFER_SIZE = 5 * 1024;
    char buffer[BUFFER_SIZE];

    // Setting read timeout

    struct timeval timeout;
    MS_TO_TIMEVAL(timeout_ms, timeout);
      if (setsockopt(socket, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout)) < 0) {
        std::cerr << "Failed to set socket timeout: " << strerror(errno) << std::endl;
        return -4;
    }

    std::ofstream outFile(file, std::ios::binary | std::ios::trunc);
      if (!outFile.is_open()) {
        std::cerr << "Failed to open file: " << file << std::endl;
        return -1;
    }

    size_t totalReceived = 0;
      while (totalReceived < size) {
        size_t toReceive = std::min(BUFFER_SIZE, size - totalReceived);
        ssize_t bytesReceived = recv(socket, buffer, toReceive, 0);
          if (bytesReceived < 0) {
              if (errno == EWOULDBLOCK || errno == EAGAIN) {
                std::cerr << "Receive timeout reached" << std::endl;
                outFile.close();
                return -5;  // timeout
              } else {
                std::cerr << "Receive error: " << strerror(errno) << std::endl;
                outFile.close();
                return -2;
              }
          } else if (bytesReceived == 0) {
            std::cerr << "Connection closed by peer" << std::endl;
            outFile.close();
            return -6;
        }
        outFile.write(buffer, bytesReceived);
          if (!outFile) {
            std::cerr << "File write error" << std::endl;
            outFile.close();
            return -3;
        }
        outFile.flush();
        totalReceived += bytesReceived;
        std::cout << "Received :" << totalReceived << " of " << size << " bytes" << std::endl;
      }
    outFile.close();
    return 0;
  }

  std::string getdate()
  {
    std::time_t t = std::time(nullptr);
    std::tm tm = *std::localtime(&t);

    std::ostringstream oss;
    oss << std::setfill('0') << std::setw(2) << tm.tm_mday << std::setw(2) << (tm.tm_mon + 1) << std::setw(2)
        << (tm.tm_year % 100) << std::setw(2) << tm.tm_hour << std::setw(2) << tm.tm_min << std::setw(2) << tm.tm_sec;

    return oss.str();
  }

  std::string readStringFile(const std::string& path)
  {
    std::ifstream file(path);
      if (!file) {
        std::cout << ("Unable to open file: " + path) << std::endl;
        return {};
    }
    std::ostringstream out_stream;
    out_stream << file.rdbuf();
    return out_stream.str();
  }

  bool writeStringFile(const std::string& path, const std::string& data)
  {
    std::ofstream outFile(path, std::ios::binary | std::ios::trunc);
      if (!outFile.is_open()) {
        std::cerr << "Failed to open file: " << path << std::endl;
        return false;
    }
    outFile << data;
    outFile.close();
    return true;
  }

  int os_recvFile(size_t size, int socket, const std::string& file, uint32_t timeout_ms)
  {
    const size_t BUFFER_SIZE = 4096;
    char buffer[BUFFER_SIZE];

    struct timeval timeout;
    timeout.tv_sec = 5;
    timeout.tv_usec = 0;
      if (setsockopt(socket, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout)) < 0) {
        std::cerr << "Failed to set socket timeout: " << strerror(errno) << std::endl;
        return -4;
    }

    int fd = open(file.c_str(), O_WRONLY | O_CREAT | O_TRUNC, 0644);
      if (fd < 0) {
        std::cerr << "Failed to open file: " << strerror(errno) << std::endl;
        return -1;
    }

    size_t totalReceived = 0;
      while (totalReceived < size) {
        size_t toReceive = std::min(BUFFER_SIZE, size - totalReceived);
        ssize_t bytesReceived = recv(socket, buffer, toReceive, 0);
          if (bytesReceived < 0) {
              if (errno == EWOULDBLOCK || errno == EAGAIN) {
                std::cerr << "Receive timeout reached" << std::endl;
                close(fd);
                return -5;
              } else {
                std::cerr << "Receive error: " << strerror(errno) << std::endl;
                close(fd);
                return -2;
              }
          } else if (bytesReceived == 0) {
            std::cerr << "Connection closed by peer" << std::endl;
            close(fd);
            return -6;
        }

        ssize_t bytesWritten = write(fd, buffer, bytesReceived);
          if (bytesWritten < 0) {
            std::cerr << "Write error: " << strerror(errno) << std::endl;
            close(fd);
            return -3;
        }
        totalReceived += bytesReceived;
      }

    close(fd);
    return 0;
  }

  void PrintFDInfo(int fd)
  {
    struct stat fd_stat;
      if (fstat(fd, &fd_stat) < 0) {
        std::cout << "ERROR: fstat(" << fd << ") failed" << std::endl;
        return;
    }
    std::cout << "File descriptor:          " << fd << "\n";
    std::cout << "File type:                ";
      switch (fd_stat.st_mode & S_IFMT) {
        case S_IFBLK: std::cout << "block device\n"; break;
        case S_IFCHR: std::cout << "character device\n"; break;
        case S_IFDIR: std::cout << "directory\n"; break;
        case S_IFIFO: std::cout << "FIFO/pipe\n"; break;
        case S_IFLNK: std::cout << "symlink\n"; break;
        case S_IFREG: std::cout << "regular file\n"; break;
        case S_IFSOCK: std::cout << "socket\n"; break;
        default: std::cout << "unknown?\n"; break;
      }

    std::cout << "I-node number:            " << static_cast<long>(fd_stat.st_ino) << "\n";

    std::cout << "Mode:                     " << std::oct << static_cast<unsigned long>(fd_stat.st_mode)
              << " (octal)\n";

    std::cout << "Link count:               " << static_cast<long>(fd_stat.st_nlink) << "\n";
    std::cout << "Ownership:                UID=" << static_cast<long>(fd_stat.st_uid)
              << "   GID=" << static_cast<long>(fd_stat.st_gid) << "\n";

    std::cout << "Preferred I/O block size: " << static_cast<long>(fd_stat.st_blksize) << " bytes\n";
    std::cout << "File size:                " << static_cast<long long>(fd_stat.st_size) << " bytes\n";
    std::cout << "Blocks allocated:         " << static_cast<long long>(fd_stat.st_blocks) << "\n";

    std::cout << "Last status change:       " << ctime(&fd_stat.st_ctime) << std::endl;
    std::cout << "Last file access:         " << ctime(&fd_stat.st_atime) << std::endl;
    std::cout << "Last file modification:   " << ctime(&fd_stat.st_mtime) << std::endl;
    std::cout << std::endl;
  }

  int ReadWholeFile(int fd, std::vector<uint8_t>* buffer)
  {
    ssize_t bytes_read{0};
    size_t block_size{1024};
    buffer->clear();
    struct stat fd_stat;
      if (fstat(fd, &fd_stat) == 0) {
        block_size = fd_stat.st_blksize;
        buffer->reserve(fd_stat.st_size);
    }
    std::vector<uint8_t> temp(block_size);
      while ((bytes_read = read(fd, temp.data(), block_size)) > 0) {
        buffer->insert(buffer->end(), temp.data(), temp.data() + bytes_read);
      }
      if (bytes_read == -1) {
        std::cout << "ERROR while reading fd=" << fd << " ERRNO: " << strerror(errno) << std::endl;
        return -1;
    }
    return buffer->size();
  }

  std::vector<uint8_t> ReadWholeFile(int fd)
  {
    std::vector<uint8_t> buffer;
    ReadWholeFile(fd, &buffer);
    return buffer;
  }


}  // namespace ucommon