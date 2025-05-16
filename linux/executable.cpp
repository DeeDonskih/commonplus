/**
 * @file executable.cpp
 * @copyright Copyright (c) 2018-2024 Dmitrii Donskikh
 * Licensed under MIT License (https://opensource.org/licenses/MIT)
 */

#include "executable.hpp"

#include "process.hpp"

#include <iostream>
#include <unistd.h>

namespace ufw
{
  Executable& Executable::operator=(const Executable& other)
  {
      if (this != &other) {
        this->m_path = other.m_path;
        this->m_pwd = other.m_pwd;
        this->m_environment = other.m_environment;
        this->m_parameters = other.m_parameters;
        resetPID();
    }
    return *this;
  }

  bool Executable::isExecutable() const
  {
    return isExecutable(m_path);
  }

  bool Executable::isRunning() const
  {
    return !isNotRunning() && uprocess::is_pid_running3(m_pid);
  }

  bool Executable::start()
  {
    std::cout << "Executable::start(): " << constructCommand() << std::endl;
    m_pid = uprocess::runBackground(constructCommand());
    return isRunning();
  }

  void Executable::kill()
  {
      if (m_pid > 0) {
        std::cout << "Killing process: " << m_pid << std::endl;
        auto childs = uprocess::getProcessChilds(m_pid);
        uprocess::killProcess(m_pid);
          for (const auto& child: childs) {
            std::cout << "Killing child: " << child << std::endl;
            uprocess::killProcess(child);
          }
    }
    m_pid = -1;
  }

  void Executable::join() const
  {
    if (isRunning()) uprocess::waitForProcess(m_pid);
  }

  std::string Executable::makeRunCommand() const
  {
    return constructCommand();
  }

  std::string Executable::constructCommand() const
  {
    std::string command{};
    if (!m_pwd.empty()) command += std::string("cd ") + m_pwd + " && ";
      for (const auto& environment: m_environment) {
        command += environment + " ";
      }
    command += m_path;
      for (const auto& param: m_parameters) {
        command += " ";
        command += param;
      }
    command += " >> log.log 2>&1 ";
    return command;
  }

  bool Executable::isExecutable(const std::string& path)
  {
    return uprocess::isExecutable(path);
  }

  //  void Executable::addEnvironment(const std::string &env) { m_environment.push_back(env); }
  //
  //  void Executable::addParameter(const std::string &param) { m_parameters.push_back(param); }
  //
  //  void Executable::addEnvironment(std::string &&env) { m_environment.push_back(env); }
  //
  //  void Executable::addParameter(std::string &&param) { m_parameters.push_back(param); }

  const std::string& Executable::workdir()
  {
    return m_pwd;
  }

  Executable::~Executable()
  {
    if (isRunning()) kill();
  }

}  // namespace ufw