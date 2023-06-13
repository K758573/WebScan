//
// Created by Shiro on 2023/6/5.
//

#ifndef ANYCODE_UTILS_H
#define ANYCODE_UTILS_H

#include <string>

#include <format>
#include <fstream>
#include <iostream>

#ifndef __FILENAME__
#define __FILENAME__ __FILE__
#endif

extern "C++" std::ofstream log_file_out;

#define LOG(...) \
  log_file_out << std::format("{} info [{}:{}]:",std::chrono::system_clock::now().time_since_epoch().count(),__FILENAME__,__LINE__) << std::format(__VA_ARGS__) << '\n'

class Utils
{
private:
  static const std::string TEMP_FILE_NAME;

public:
  static std::wstring u8stringToWstring(std::string &str);
  
  static std::string wstringToU8string(std::wstring &wstr);
  
  static std::wstring formatErrorMessage(unsigned long error_code);
  
  static std::string downloadUrlToString(const std::string &url);
};

#endif //ANYCODE_UTILS_H
