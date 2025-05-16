/**
 * @file process.cpp
 * @author Dmitrii Donskikh (deedonskihdev@gmail.com)
 * @brief Implements process control allowing launching, monitoring and terminating external programs
 * @version 0.1
 * @date 2018-06-08
 *
 * @copyright Copyright (c) 2018-2024 Dmitrii Donskikh
 *            Licensed under MIT License (https://opensource.org/licenses/MIT)
 */

#include "process.hpp"

#include <array>
#include <cerrno>
#include <csignal>
#include <iostream>
#include <string>
#include <sys/stat.h>
#include <sys/wait.h>
#include <unistd.h>
#include <vector>

#if __has_include(<filesystem>)
#define HAVE_FILESYSTEM_HEADER
#include <algorithm>
#include <filesystem>
#include <fstream>
namespace fs = std::filesystem;
#else
// #warning "The <filesystem> header is not available with your compiler."
#endif

namespace uprocess
{
  static void execute(const std::string& command, int& childpid, int type)
  {
    int result_pid = fork();
      if (result_pid == -1) {
        // fork() call error
        std::cerr << "Fork failed!" << std::endl;
        return;
    }
      if (result_pid == 0) {
        // Here is child process
        // Change current process with our shell command
        execl("/bin/sh", "sh", "-c", command.c_str(), (char*)nullptr);
        // execl returns in case of error
        std::cerr << "Exec failed!" << std::endl;
        _exit(1);  // Exit from child
      } else {
        childpid = result_pid;
        if (type != ExecType::Background) waitForProcess(childpid);
      }
  }

  bool isExecutable(const std::string& path)
  {
    return access(path.c_str(), X_OK) == 0;
  }

  int runBackground(const std::string& command)
  {
    int pid;
    execute(command, pid, ExecType::Background);
    // return process ID
    return pid;
  }

  void waitForProcess(int pid)
  {
    int status;
    pid_t result = waitpid(pid, &status, 0);  // waiting

      if (result == -1) {
        // waitpid error
        std::cerr << "Failed to wait for process with PID: " << pid << std::endl;
      } else if (WIFEXITED(status)) {
        // Normal exited process
        int exitStatus = WEXITSTATUS(status);
        std::cout << "Process with PID " << pid << " exited with status: " << exitStatus << std::endl;
      } else if (WIFSIGNALED(status)) {
        // Signaled
        int signal = WTERMSIG(status);
        std::cout << "Process with PID " << pid << " was terminated by signal: " << signal << std::endl;
      } else {
        std::cout << "Process with PID " << pid << " did not exit normally." << std::endl;
      }
  }

  int runForeground(const std::string& command)
  {
    int pid;
    execute(command, pid, ExecType::Foreground);
    return pid;
  }

  int runForeground(const std::string& command, int& childpid, std::ostream* output)
  {
      if (output == nullptr) {
        execute(command, childpid, ExecType::Foreground);
        return childpid;
    }

    std::array<int, 2> pipefd{};
      // Create the pipe
      if (pipe(pipefd.data()) == -1) {
        throw std::runtime_error("Failed to create pipe");
    }
    // Create child process
    childpid = fork();
      if (childpid == -1) {
        // throw std::runtime_error("Failed to fork");
        return childpid;
    }
      // Child process
      if (childpid == 0) {
        // Close read end of the pipe, we don't need it in the child process
        close(pipefd[0]);
        // Redirect stdout to write end of the pipe
        dup2(pipefd[1], STDOUT_FILENO);
          // Replace current process image with "sh -c command"
          if (execl("/bin/sh", "sh", "-c", command.c_str(), nullptr) < 0) {
            throw std::runtime_error("Failed to execute command");
        }
        _exit(1);  // Exit from child
      }
      // Parent process
      else {
        // Close write end of the pipe, we don't need it in the parent process
        close(pipefd[1]);
          // If outputStream is not null, write the output to the stream
          if (output) {
            constexpr size_t bufferSize = 1024;
            std::vector<char> buffer(bufferSize);
            ssize_t bytesRead = 0;
              while ((bytesRead = read(pipefd[0], buffer.data(), bufferSize)) != 0) {
                  if (bytesRead == -1) {
                    std::cerr << "Failed to read from pipe" << std::endl;
                    break;
                }
                output->write(buffer.data(), bytesRead);
              }
        }
        waitForProcess(childpid);
      }
    return childpid;
  }

  bool isRunning(int pid)
  {
      // Send a null signal to the process
      if (::kill(pid, 0) == -1) {
          // ESRCH is the only error that signifies that the process is not running
          if (errno == ESRCH) {
            return false;
          } else {
            return true;
          }
      } else {
        return true;
      }
  }

  void killProcess(int pid)
  {
    ::kill(pid, SIGKILL);
  }

  std::vector<pid_t> getProcessChilds(pid_t parent_pid)
  {
    std::vector<pid_t> child_pids;

      for (const auto& entry: fs::directory_iterator("/proc")) {
          if (entry.is_directory()) {
            std::string pid_str = entry.path().filename();

              // Check is DIR name is PID
              if (std::all_of(pid_str.begin(), pid_str.end(), ::isdigit)) {
                std::ifstream status_file(entry.path() / "status");
                std::string line;

                  while (std::getline(status_file, line)) {
                      if (line.rfind("PPid:", 0) == 0) {
                        pid_t ppid = std::stoi(line.substr(5));
                          if (ppid == parent_pid) {
                            child_pids.push_back(std::stoi(pid_str));
                        }
                        break;
                    }
                  }
            }
        }
      }
    return child_pids;
  }

  bool is_pid_running2(pid_t pid)
  {
    if (pid <= 0) return false;

    std::string path = "/proc/" + std::to_string(pid);
    struct stat statbuf;
    return (stat(path.c_str(), &statbuf) == 0);
  }

  bool is_pid_running3(pid_t pid)
  {
    return (getpgid(pid) >= 0 || errno != ESRCH);
  }

}  // namespace uprocess