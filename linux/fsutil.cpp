/**
 * @file fsutil.cpp
 * @copyright Copyright (c) 2018-2024 Dmitrii Donskikh
 * Licensed under MIT License (https://opensource.org/licenses/MIT)
 */
#include "fsutil.hpp"

#include <filesystem>
#include <fstream>
#include <iostream>
#include <limits.h>
#include <regex>
#include <system_error>
#include <unistd.h>

namespace fs = std::filesystem;

namespace ucommon
{

  bool hasDirectory(const std::string& path) noexcept
  {
    std::error_code ec;
    return fs::exists(path, ec) && fs::is_directory(path, ec);
  }

  bool hasFile(const std::string& path) noexcept
  {
    std::error_code ec;
    return fs::exists(path, ec) && fs::is_regular_file(path, ec);
  }

  std::list<std::string> getSubdirs(const std::string& path)
  {
    std::list<std::string> subdirs;
    if (!hasDirectory(path)) return subdirs;

    std::error_code ec;
      for (const auto& entry: fs::directory_iterator(path, ec)) {
          if (entry.is_directory(ec)) {
            // subdirs.push_back(entry.path().filename().string());
            subdirs.push_back(entry.path().string());
        }
      }

    return subdirs;
  }

  std::list<std::string> getFiles(const std::string& path)
  {
    std::list<std::string> files;
    if (!hasDirectory(path)) return files;
    std::error_code ec;
      for (const auto& entry: fs::directory_iterator(path, ec)) {
          if (entry.is_regular_file(ec)) {
            files.push_back(entry.path().filename().string());
        }
      }
    return files;
  }

  std::string wildcardToRegex(const std::string& pattern)
  {
    std::string regex;
      for (char ch: pattern) {
          switch (ch) {
            case '*': regex += ".*"; break;
            case '?': regex += "."; break;
            case '.': regex += "\\."; break;
            default: regex += ch;
          }
      }
    return regex;
  }

  std::list<std::string> getFiles(const std::string& path, const std::string& pattern)
  {
    std::list<std::string> matchedFiles;
    if (!hasDirectory(path)) return matchedFiles;

    std::regex re(wildcardToRegex(pattern), std::regex::icase);

      for (const auto& entry: fs::directory_iterator(path)) {
          if (entry.is_regular_file()) {
            std::string filename = entry.path().filename().string();
              if (std::regex_match(filename, re)) {
                matchedFiles.push_back(filename);
            }
        }
      }
    return matchedFiles;
  }

  int mkdir(const std::string& path) noexcept
  {
    std::error_code ec;
      if (fs::create_directory(path, ec)) {
        return 0;
      } else {
        return ec.value();
      }
  }

  int mkdir_p(const std::string& path) noexcept
  {
    std::error_code ec;
      if (fs::create_directories(path, ec)) {
        return 0;
      } else {
        return ec.value();
      }
  }

  std::string pwd()
  {
    char buffer[PATH_MAX];
      if (getcwd(buffer, sizeof(buffer)) != nullptr) {
        return std::string(buffer);
      } else {
        perror("getcwd");
        return "";
      }
  }

  int setExecutable(const std::string& path) noexcept
  {
    std::error_code ec;

    fs::permissions(path, fs::perms::owner_exec | fs::perms::group_exec | fs::perms::others_exec, fs::perm_options::add,
                    ec);

    return ec.value();
  }

  bool isExecutable(const std::string& path) noexcept
  {
    std::error_code ec;
    fs::perms p = fs::status(path, ec).permissions();

    if (ec) return false;

    return (p & fs::perms::owner_exec) != fs::perms::none;
  }

  int removeFile(const std::string& path) noexcept
  {
    std::error_code ec;
      if (fs::remove(path, ec)) {
        return 0;
      } else {
        return ec.value();
      }
  }

  int removeDir(const std::string& path) noexcept
  {
    std::cout << "Removing directory: " << path << std::endl;
    std::error_code ec;
    fs::remove_all(path, ec);
    return ec.value();
  }

  std::string stripFilename(const std::string& fullPath)
  {
    fs::path p(fullPath);
    return p.parent_path().string();
  }

  int touch(const std::string& path) noexcept
  {
    std::error_code ec;
      if (!fs::exists(path, ec)) {
        std::ofstream file(path);
          if (!file) {
            return -1;
        }
      } else {
        fs::file_time_type now = fs::file_time_type::clock::now();
        fs::last_write_time(path, now, ec);
          if (ec) {
            return ec.value();
        }
      }
    return 0;
  }

  std::list<std::string> getFilesWithExtention(const std::string& path, const std::string& extension) noexcept
  {
    std::list<std::string> files;
    std::error_code ec;
    if (!hasDirectory(path)) return files;

      for (const auto& entry: fs::directory_iterator(path, ec)) {
          if (entry.is_regular_file(ec)) {
            std::string filename = entry.path().string();
              if (filename.length() >= extension.length() &&
                  filename.compare(filename.length() - extension.length(), extension.length(), extension) == 0) {
                files.push_back(filename);
            }
        }
      }
    return files;
  }

}  // namespace ucommon