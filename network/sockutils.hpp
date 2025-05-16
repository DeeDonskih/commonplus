/**
 * @file sockutils.hpp
 * @author Dmitrii Donskikh (deedonskihdev@gmail.com)
 * @brief Socket-related utility functions for handling and inspecting socket file descriptors
 * @brief Provides functionality for working with socket types, addresses, and connection states
 * @version 0.1
 * @date 2019-04-16
 *
 * @copyright Copyright (c) 2019-2024 Dmitrii Donskikh
 *            Licensed under MIT License (https://opensource.org/licenses/MIT)
 */

#ifndef SOCKUTILS_HPP
#define SOCKUTILS_HPP

#include <optional>
#include <string>
#include <utility>

namespace ucommon
{

  /**
   * @enum SocketFamily
   * @brief Represents the protocol family of a socket.
   *
   * This enum is used to categorize the type of a socket based on its addressing scheme.
   *
   * @value IPv4 Represents an IPv4 socket address family.
   * @value IPv6 Represents an IPv6 socket address family.
   * @value Unknown Represents an unknown or unsupported socket address family.
   */
  enum class SocketFamily
  {
    IPv4,
    IPv6,
    Unknown
  };

  /**
   * Enum class representing the type of a socket.
   *
   * - Stream: Represents a stream-oriented socket, typically used for TCP connections.
   * - Datagram: Represents a datagram-oriented socket, often used for UDP connections.
   * - Raw: Represents a raw socket, which provides low-level access to network protocols.
   * - Unknown: Represents an unknown or unsupported socket type.
   */
  enum class SocketType
  {
    Stream,
    Datagram,
    Raw,
    Unknown
  };

  /**
   * @brief Checks if the given file descriptor is currently open.
   *
   * This function uses the `fstat` system call to determine whether the file
   * descriptor refers to an open file or socket. If the system call succeeds,
   * it indicates that the file descriptor is valid and open.
   *
   * @param fd The file descriptor to check.
   * @return `true` if the file descriptor is open, `false` otherwise.
   */
  bool IsFdOpen(int fd);
  /**
   * Checks if the given file descriptor represents a socket.
   *
   * This function uses `fstat` to determine if the file descriptor is valid
   * and checks the file mode to verify if it is a socket.
   *
   * @param fd The file descriptor to check.
   * @return True if the file descriptor represents a socket, false otherwise.
   */
  bool IsSocket(int fd);
  /**
   * Retrieves the socket family and type for a given file descriptor.
   *
   * @param fd File descriptor of the socket for which the type is being queried.
   * @return A pair containing the socket family (SocketFamily) and the socket type
   *         (SocketType) wrapped in a std::optional, or std::nullopt if the
   *         information cannot be determined (e.g., the file descriptor is
   *         invalid or does not represent a socket).
   *
   * The function determines the socket family (IPv4, IPv6, or Unknown) and type
   * (Stream, Datagram, Raw, or Unknown) by querying the socket options and
   * socket address information via `getsockopt` and `getsockname`.
   */
  std::optional<std::pair<SocketFamily, SocketType>> GetSocketType(int fd);
  /**
   * Checks if a TCP connection is established on the given file descriptor.
   *
   * @param fd The file descriptor to check.
   * @return true if the TCP connection is in the established state, false otherwise.
   */
  bool IsTcpConnected(int fd);
  /**
   * Retrieves the address and port of the peer connected to a given socket file descriptor.
   *
   * This function uses the `getpeername` system call to obtain the address
   * and port of the peer connected to the socket identified by `fd`.
   * If the socket is not connected or if the address cannot be retrieved,
   * the function returns `std::nullopt`.
   *
   * Supported address families include:
   * - IPv4 (AF_INET)
   * - IPv6 (AF_INET6)
   *
   * The address is returned as a string representation (e.g., "192.168.1.1" for IPv4)
   * and the port number in a `std::pair`.
   *
   * @param fd The file descriptor associated with the socket.
   * @return A `std::optional` containing a `std::pair` of the peer's address (string)
   *         and port (integer). Returns `std::nullopt` if the peer address cannot
   *         be determined.
   */
  std::optional<std::pair<std::string, int>> GetPeerAddress(int fd);
  /**
   * Retrieves the local address and port associated with a socket file descriptor.
   *
   * The function extracts the address and port of the local endpoint of the socket
   * represented by the file descriptor. This information is useful to determine
   * which address and port have been assigned to the socket.
   *
   * If the provided file descriptor does not correspond to a valid socket or an
   * error occurs while fetching the local address, the function returns an empty
   * optional.
   *
   * @param fd The file descriptor of the socket.
   * @return A `std::optional` containing a `std::pair` of the local IP address
   * (in string format) and port number if successful. Returns an empty
   * optional on failure.
   */
  std::optional<std::pair<std::string, int>> GetLocalAddress(int fd);
  /**
   * Retrieves the local hostname of the system.
   *
   * This method attempts to obtain the local hostname. If the hostname retrieval fails,
   * it returns an empty std::optional.
   *
   * @return An optional string containing the local hostname if successful; otherwise, std::nullopt.
   */
  std::optional<std::string> GetLocalHostname();
  /**
   * Retrieves the local IP address of the machine.
   *
   * This method attempts to determine the local IPv4 or IPv6 address
   * associated with the machine. The IP address is returned as a string
   * if it can be successfully retrieved. Otherwise, it returns an empty
   * optional to signify failure or unavailability.
   *
   * @return The local IP address as a string if successful, or an empty optional if unable to determine the address.
   */
  std::optional<std::string> GetLocalIp();

}  // namespace ucommon


#endif  // SOCKUTILS_HPP
