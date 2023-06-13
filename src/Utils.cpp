//
// Created by Shiro on 2023/6/5.
//

#include "Utils.h"
#include "Windows.h"

const std::string Utils::TEMP_FILE_NAME = "temp.txt";

std::string Utils::downloadUrlToString(const std::string &url)
{
  std::string answer;
  auto res = URLDownloadToFileA(nullptr, url.c_str(), TEMP_FILE_NAME.c_str(), 0, nullptr);
  if (res != S_OK)
    return "";
  std::ifstream in(TEMP_FILE_NAME, std::ios::binary | std::ios::in);
  in.seekg(0, std::ios::end);
  int file_size = (int) in.tellg();
  in.seekg(0, std::ios::beg);
  answer.resize(file_size);
  in.read(&answer[0], file_size);
  in.close();
  DeleteFile(TEMP_FILE_NAME.c_str());
  return answer;
}

std::wstring Utils::u8stringToWstring(std::string &str)
{
  auto size_needed = MultiByteToWideChar(CP_UTF8, 0, str.c_str(), (int) str.size(), nullptr, 0);
  std::wstring wstr(size_needed, 0);
  MultiByteToWideChar(CP_UTF8, 0, str.c_str(), (int) str.size(), &wstr[0], (int) wstr.size());
  return wstr;
}

std::string Utils::wstringToU8string(std::wstring &wstr)
{
  auto size_needed = WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), (int) wstr.size(), nullptr, 0, nullptr, nullptr);
  std::string str(size_needed, 0);
  WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), (int) wstr.size(), &str[0], (int) str.size(), nullptr, nullptr);
  return str;
}

std::wstring Utils::formatErrorMessage(unsigned long error_code)
{
  std::wstring str(L"a");
  wchar_t *temp;
  FormatMessageW(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
                 nullptr,
                 error_code,
                 0,
                 (LPWSTR) &temp,
                 0,
                 nullptr);
  str.assign(temp);
  return str;
}
