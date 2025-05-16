/**
 * @file utils.hpp
 * @author Dmitrii Donskikh (deedonskihdev@gmail.com)
 * @brief Utility functions and classes for file, system, and platform operations.
 *
 * This header provides various utilities for file I/O, system interaction,
 * platform detection, and resource management. Utilities include file reading/writing,
 * extraction operations, platform-specific checks, pixel format enumeration,
 * scope guards for RAII-style management, and FD info display facilities.
 *
 * Designed to simplify common tasks in Unix-like and cross-platform environments.
 *
 * @version 0.1
 * @date 2018-10-30
 *
 * @copyright Copyright (c) 2018-2024 Dmitrii Donskikh
 *            Licensed under the MIT License (https://opensource.org/licenses/MIT)
 */

#ifndef UFW_UNIX_COMMON_UTILS_HPP_19
#define UFW_UNIX_COMMON_UTILS_HPP_19

#include <cstdint>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <string>
#include <unistd.h>
#include <vector>

// Check is NEON extension for arm cpus are available
#if defined(__ARM_NEON) || defined(__ARM_NEON__)
#define HAS_NEON
#endif


namespace ucommon
{
  /**
   * Extracts the contents of a tar archive to a specified directory
   * @param filename The path to the tar file to be extracted.
   * @param to The directory where the contents of the tar file will be extracted.
   * @return Returns 0 if the extraction was successful, or a non-zero value if an error occurred.
   * @note This function uses a system command to extract the tar file, so it depends
   *       on the availability of the `tar` command in the system's environment.
   * @exception noexcept This function does not throw exceptions.
   */
  int untar(const std::string& filename, const std::string& to) noexcept;

  /**
   * @brief Receives a file over a socket connection with a specified size and timeout.
   *
   * This function reads a specified number of bytes from the given socket and writes the
   * data to a file on disk. It also handles timeout settings and error scenarios.
   *
   * @param size The expected size of the file to be received, in bytes.
   * @param socket The socket file descriptor used for receiving data.
   * @param file The path to the file where the received data will be saved.
   * @param timeout_ms The socket read timeout in milliseconds.
   *
   * @return
   *     0 on success.
   *    -1 if the file could not be opened for writing.
   *    -2 if there was an error while receiving data from the socket.
   *    -3 if there was an error writing the received data to the file.
   *    -4 if setting the socket timeout failed.
   *    -5 if the receive operation timed out.
   *    -6 if the connection was closed by the peer before receiving the full file.
   */
  int recvFile(size_t size, int socket, const std::string& file, uint32_t timeout_ms = 3000);

  /**
   * Receives a file over a network socket and writes it to the specified path on the local filesystem.
   *
   * @param size The total size of the file to be received, in bytes.
   * @param socket The socket file descriptor used for receiving data.
   * @param file The path to the local file where the received data will be written.
   * @param timeout_ms The timeout duration in milliseconds for the receive operation.
   * @return Returns 0 on success, or a negative error code on failure:
   *         - -1 if the file could not be opened for writing.
   *         - -2 if a receive error occurred.
   *         - -3 if a write error occurred on the local file.
   *         - -4 if there was an error setting the socket timeout.
   *         - -5 if the receive operation timed out.
   *         - -6 if the connection was closed by the peer before the full file was received.
   */
  int os_recvFile(size_t size, int socket, const std::string& file, uint32_t timeout_ms = 3000);

  /**
   * @brief Retrieves the current date and time as a formatted string.
   *
   * This function returns the current system date and time in the following
   * format: DDMMYYHHMMSS, where:
   * - DD is the day of the month, zero-padded to 2 digits.
   * - MM is the month (1-based), zero-padded to 2 digits.
   * - YY is the last two digits of the year.
   * - HH is the hour in 24-hour format, zero-padded to 2 digits.
   * - MM is the minute, zero-padded to 2 digits.
   * - SS is the seconds, zero-padded to 2 digits.
   *
   * @return A string representing the current date and time in the format DDMMYYHHMMSS.
   */
  std::string getdate();

  /**
   * Reads the entire content of a file into a string.
   *
   * This function opens the file specified by the given file path, reads
   * its contents and returns it as a single string. If the file cannot
   * be opened, an empty string is returned and a corresponding message
   * is printed to the standard output.
   *
   * @param path The path to the file to be read.
   * @return A string containing the contents of the file, or an empty
   *         string if the file cannot be opened.
   */
  std::string readStringFile(const std::string& path);
  /**
   * Writes a string to a file at the specified path.
   *
   * @param path The file path where the string should be written.
   * @param data The string data to be written to the file.
   * @return True if the file was successfully written; otherwise, false.
   */
  bool writeStringFile(const std::string& path, const std::string& data);


  /**
   * @enum PixelFormat
   * @brief Enumeration representing pixel formats.
   *
   * This enumeration is used to specify different pixel formats.
   */
  enum PixelFormat
  {
    ARGB8888 = (int)0,
    XRGB8888 = (int)1
  };

  /**
   * Checks whether the NEON SIMD extensions are available.
   *
   * @return true if the system supports NEON, false otherwise.
   */
  constexpr bool hasNeon()
  {
#ifdef HAS_NEON
    return true;
#else
    return false;
#endif
  }

  /**
   * @brief Prints detailed information about the specified file descriptor.
   *
   * This function retrieves and displays information such as file type,
   * ownership, size, and time attributes of the file descriptor using `fstat`.
   *
   * @param fd File descriptor whose information is to be printed.
   */
  void PrintFDInfo(int fd);
  /**
   * Reads the entire contents of a file descriptor into a buffer.
   *
   * @param fd The file descriptor to read from.
   * @param buffer A pointer to a vector where the read data will be stored.
   *               The vector will be cleared before appending the data.
   * @return The total number of bytes read and stored in the buffer, or -1 if an error occurs.
   */
  int ReadWholeFile(int fd, std::vector<uint8_t>* buffer);
  /**
   * Reads the entire content of a file descriptor into a byte buffer.
   *
   * @param fd The file descriptor to read from.
   * @return A vector containing the contents of the file descriptor.
   */
  std::vector<uint8_t> ReadWholeFile(int fd);
  /**
   * Reads the entire content of a file as a string.
   *
   * @param path The file path to be read.
   * @return A string containing the contents of the file. If the file cannot be opened,
   *         an empty string is returned.
   */
  std::string readStringFile(const std::string& path);

  /**
   * Writes the contents of the given data vector to a binary file with the specified name.
   *
   * The function creates or overwrites the file with the given name, writes the binary
   * representation of the vector's contents to it, and closes the file. If the file cannot
   * be opened or there is an error during writing, the function returns an error code.
   *
   * @tparam T The type of elements stored in the data vector. This type must be trivially copyable
   *           to ensure proper binary serialization to the file.
   * @param name The name (including path) of the file to write the data to.
   * @param data A vector containing the data to be written to the file.
   *             The entire content of the vector is serialized as binary data.
   * @return The total number of bytes written to the file on success.
   *         Returns -1 if there is an error opening the file or writing to it.
   */
  template<typename T>
  int dumpToFile(const std::string& name, const std::vector<T>& data)
  {
    std::ofstream outfile(name, std::ios::binary);
      if (!outfile.is_open()) {
        std::cerr << "Error opening file: " << name << std::endl;
        return -1;
    }
    outfile.write(reinterpret_cast<const char*>(data.data()), data.size() * sizeof(T));
      if (!outfile) {
        std::cerr << "Error writing to file: " << name << std::endl;
        return -1;
    }
    outfile.close();
    return data.size() * sizeof(T);
  }


  // A simple scope guard that calls a provided callable on destruction, unless released.
  /**
   * @class ScopeGuard
   *
   * @brief A utility class that ensures the execution of a callable object upon its destruction, unless explicitly
   * released.
   *
   * This class helps manage resource cleanup within a scope by providing a mechanism
   * to automatically execute a callable object (e.g., lambda, function) when the
   * ScopeGuard instance is destroyed. It is particularly useful for exception safety
   * and ensuring resource correctness in complex functions.
   */
  template<typename F>
  class ScopeGuard
  {
    /**
     * Constructs a ScopeGuard object which ensures the provided callable will be
     * executed upon the destruction of the ScopeGuard object unless explicitly released.
     *
     * @param f A callable object to be executed upon destruction of the ScopeGuard.
     *          This is moved into the ScopeGuard object. It must satisfy the
     *          nothrow-move-constructible requirement.
     */
  public:
    explicit ScopeGuard(F&& f) noexcept(std::is_nothrow_move_constructible<F>::value):
        func_(std::move(f)), active_(true)
    {}
    ~ScopeGuard() noexcept
    {
        if (active_) {
            try {
              func_();
            } catch (...) {}
      }
    }
    /**
     * Move constructor for the ScopeGuard class.
     * Transfers ownership of the cleanup function from the other ScopeGuard
     * object to the newly constructed one, leaving the other object inactive.
     */
    ScopeGuard(ScopeGuard&& other) noexcept: func_(std::move(other.func_)), active_(other.active_)
    {
      other.release();
    }

    ScopeGuard(const ScopeGuard&) = delete;
    ScopeGuard& operator=(const ScopeGuard&) = delete;
    /**
     * @brief Disables the active state of the ScopeGuard, preventing the stored function
     *        from being called upon destruction.
     */
    void release() noexcept
    {
      active_ = false;
    }

  private:
    F func_;
    bool active_;
  };

  /**
   * Creates a `ScopeGuard` object that ensures the execution of a provided callable
   * when the `ScopeGuard` goes out of scope, unless explicitly released.
   *
   * @tparam F The type of the callable object to be executed.
   * @param f The callable object to execute when the `ScopeGuard` is destructed.
   * @return A `ScopeGuard` object that manages the lifetime of the callable.
   */
  template<typename F>
  ScopeGuard<std::decay_t<F>>
  make_scope_guard(F&& f) noexcept(std::is_nothrow_move_constructible<std::decay_t<F>>::value)
  {
    return ScopeGuard<std::decay_t<F>>(std::forward<F>(f));
  }

  template<typename F, typename... Args>
  /**
   * @brief  Creates a scope guard that ensures the provided callable object @param f is executed with the given
   * arguments @param args when the guard goes out of scope, unless explicitly released.
   */
  auto make_scope_guard(F&& f,
                        Args&&... args) noexcept(std::is_nothrow_move_constructible<std::decay_t<F>>::value &&
                                                 (std::is_nothrow_move_constructible<std::decay_t<Args>>::value && ...))
  {
    auto bound = [func = std::forward<F>(f),
                  tup = std::tuple<std::decay_t<Args>...>(std::forward<Args>(args)...)]() mutable {
      std::apply(func, tup);
    };
    return make_scope_guard(std::move(bound));
  }

}  // namespace ucommon
#endif  // UFW_UNIX_COMMON_UTILS_HPP_19