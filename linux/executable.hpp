/**
 * @file executable.hpp
 * @author Dmitrii Donskikh (deedonskihdev@gmail.com)
 * @brief Implements process control allowing launching, monitoring and terminating external programs
 * @version 0.1
 * @date 2018-10-09
 *
 * @copyright Copyright (c) 2018-2024 Dmitrii Donskikh
 *            Licensed under MIT License (https://opensource.org/licenses/MIT)
 */

#ifndef UFW_EXECUTABLE_HPP
#define UFW_EXECUTABLE_HPP

#include <string>
#include <utility>
#include <vector>

namespace ufw
{

  class Executable
  {
    std::string m_path{};
    std::string m_pwd{};
    std::vector<std::string> m_environment{};
    std::vector<std::string> m_parameters{};
    int m_pid{-1};

    bool isNotRunning() const
    {
      return m_pid <= 0;
    }

    void resetPID()
    {
      m_pid = -1;
    }

    std::string constructCommand() const;

  public:
    Executable() = default;

    template<class PT, class = std::enable_if_t<!std::is_same_v<std::decay_t<PT>, Executable>>>
    Executable(PT&& path): m_path(std::forward<PT>(path)){};

    template<class PT, class ET, class PART>
    Executable(PT&& path, ET&& environment, PART&& parameters):
        m_path(std::forward<PT>(path)), m_environment(std::forward<ET>(environment)),
        m_parameters(std::forward<PART>(parameters))
    {}

    Executable(const Executable& other):
        m_path(other.m_path), m_pwd(other.m_pwd), m_environment(other.m_environment), m_parameters(other.m_parameters),
        m_pid(-1)
    {}

    Executable(Executable&& other) = default;
    Executable& operator=(const Executable& other);
    Executable& operator=(Executable&& other) = default;

    ~Executable();

    bool isExecutable() const;
    bool isRunning() const;

    template<class T>
    void addEnvironment(T&& env)
    {
      m_environment.push_back(std::forward<T>(env));
    }

    template<class T>
    void addParameter(T&& param)
    {
      m_parameters.push_back(std::forward<T>(param));
    }

    //    void addEnvironment(const std::string &env);
    //    void addParameter(const std::string &param);
    //    void addEnvironment(std::string &&env);
    //    void addParameter(std::string &&param);

    template<typename T>
    void setWorkdir(T&& value)
    {
      m_pwd = std::forward<T>(value);
    }
    const std::string& workdir();

    bool start();
    void kill();
    void join() const;

    std::string makeRunCommand() const;

    static bool isExecutable(const std::string& path);

    //  bool operator==(const Executable &other) const;
    //  bool operator!=(const Executable &other) const;
  };

}  // namespace ufw

#endif  // UFW_EXECUTABLE_HPP
