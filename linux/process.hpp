/**
 * @file   process.hpp
 * @brief  Process management utilities.
 * @author Dmitrii Donskikh
 * @date   2018
 * @copyright (c) 2018 Dmitrii Donskikh
 *
 * SPDX-License-Identifier: MIT
 */
#pragma once

#include <iostream>
#include <string>
#include <vector>

namespace uprocess
{
  enum ExecType
  {
    Background = (int)0,
    Foreground
  };

  /**
   * @brief Checks if a file at the given path is executable.
   *
   * This function determines if a file at the specified path is executable
   * by attempting to execute the file using the operating system's method.
   * It checks the platform-specific file permissions or other relevant
   * attributes to determine executability.
   *
   * @param path The path to the file to be checked.
   *
   * @return true if the file is executable, false otherwise.
   *
   * @note This function only checks if a file is executable, it does not
   *       check if the file actually exists at the given path.
   */
  bool isExecutable(const std::string& path);

  /**
   * @brief Runs a specified command in the background.
   *
   * This function executes a command in the background and does not wait for its completion.
   * The command to be executed is provided as a string parameter.
   *
   * @param command The command to be executed in the background.
   *
   * @note The command should be a valid shell command.
   */
  int runBackground(const std::string& command);
  /**
   * @brief Runs a command in the foreground and waits for it to complete.
   *
   * This function runs the given command in the foreground, meaning it waits for the command to complete
   * before returning. It takes a command as input, specified as a string.
   *
   * @param command The command to be executed in the foreground.
   */
  int runForeground(const std::string& command);

  /**
   * @brief Executes a command in the foreground and waits for it to finish.
   *
   * This function takes a command as a string and executes it in the foreground.
   * It also provides the option to redirect the output to a given output stream.
   *
   * @param command The command to be executed in the foreground.
   * @param childpid A reference to an integer variable to store the child process ID.
   * @param output (Optional) A pointer to an output stream to redirect the output. Default is nullptr, which means no
   * redirection.
   *
   * @return The exit status code of the executed command.
   */
  int runForeground(const std::string& command, int& childpid, std::ostream* output = nullptr);

  /**
   * @brief Wait for a process to exit.
   *
   * This function waits for a process with the given process ID (PID) to exit. It blocks until the process exits
   * or until an error occurs.
   *
   * @param pid The process ID of the process to wait for.
   */
  void waitForProcess(int pid);

  /**
   * @brief Checks if a process with the given process ID (pid) is running.
   *
   * @param pid The process ID of the process to check.
   * @return true if the process is running, false otherwise.
   *
   * This function checks if a process with the given process ID (pid) is running. It does this
   * by sending a null signal to the process. If the kill system call returns -1 and the errno
   * variable is ESRCH, it means that the process is not running, in which case the function
   * returns false. Otherwise, it returns true to indicate that the process is running.
   */
  bool isRunning(int pid);

  /**
   * @brief Kill a process.
   *
   * This function is used to kill a process identified by its process ID (PID).
   * The process is terminated by sending the SIGKILL signal to it.
   *
   * @param pid The process ID of the process to be killed.
   *
   * @note Make sure to use this function responsibly, as terminating a process abruptly
   * can result in unexpected behavior and data loss.
   */
  void killProcess(int pid);

  /**
   * @brief Retrieves a list of child process IDs for a given parent process ID.
   *
   * This function takes a parent process ID (PID) as input and returns a vector containing
   * the PIDs of all child processes spawned by the given parent process. If the parent
   * PID does not have any child processes, the returned vector will be empty.
   *
   * @param parent_pid The process ID of the parent process whose child process IDs are to be retrieved.
   * @return A vector of process IDs representing the child processes of the given parent process.
   */
  std::vector<pid_t> getProcessChilds(pid_t parent_pid);

  bool is_pid_running2(pid_t pid);
  bool is_pid_running3(pid_t pid);

}  // namespace uprocess
