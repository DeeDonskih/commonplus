/**
 * @file fsutil.hpp
 * @author Dmitrii Donskikh (deedonskihdev@gmail.com)
 * @brief Filesystem utilities for Unix-like systems providing directory and file operations
 * @version 0.1
 * @date 2018-10-27
 *
 * @copyright Copyright (c) 2018-2024 Dmitrii Donskikh
 *            Licensed under MIT License (https://opensource.org/licenses/MIT)
 */

#ifndef UFW_UNIXFSUTIL_HPP
#define UFW_UNIXFSUTIL_HPP

#include <list>
#include <string>

namespace ucommon
{
  bool hasDirectory(const std::string& path) noexcept;
  bool hasFile(const std::string& path) noexcept;

  std::list<std::string> getSubdirs(const std::string& path);
  std::list<std::string> getFiles(const std::string& path);
  std::list<std::string> getFiles(const std::string& path, const std::string& pattern);
  std::list<std::string> getFilesWithExtention(const std::string& path, const std::string& extension) noexcept;

  int mkdir(const std::string& path) noexcept;
  int mkdir_p(const std::string& path) noexcept;

  std::string pwd();

  int setExecutable(const std::string& path) noexcept;
  bool isExecutable(const std::string& path) noexcept;

  int removeFile(const std::string& path) noexcept;
  int removeDir(const std::string& path) noexcept;
  std::string stripFilename(const std::string& fullPath);
  int touch(const std::string& path) noexcept;
}  // namespace fsutil

#endif  // UFW_UNIXFSUTIL_HPP