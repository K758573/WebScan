//
// Created by Shiro on 2023/6/13.
//

#ifndef WEBSCAN_HTML_H
#define WEBSCAN_HTML_H

#include <vector>
#include <string>
#include <unordered_map>
#include <ostream>
#include <array>

/**
 * 表单，包含请求地址(从http:开始计算)，请求方式，请求参数
 */
class Form
{
public:
  static const std::string CONTENT_TYPE;
  std::string method;
  std::string action;
  std::unordered_map<std::string, std::string> args;
  
  friend std::ostream &operator<<(std::ostream &os, const Form &form);
  
};

class CookieItem
{
private:
  static const int COOKIE_ITEM_NUMBER = 7;
  std::array<std::string, COOKIE_ITEM_NUMBER> args;

public:
  [[nodiscard]] const std::string &tail() const;
  
  [[nodiscard]] const std::string &path() const;
  
  [[nodiscard]] const std::string &secure() const;
  
  [[nodiscard]] const std::string &expires() const;
  
  [[nodiscard]] const std::string &name() const;
  
  [[nodiscard]] const std::string &value() const;
  
  
  [[nodiscard]] const std::string &domain() const;
  
  [[nodiscard]] std::string tostring() const;
  
  void value(std::string value)
  {
    args[6] = std::move(value);
  };

private:
  std::string cookie_list_item_;
public:
  static CookieItem fromstring(const std::string &cookie_list_item);
  
  friend std::ostream &operator<<(std::ostream &os, const CookieItem &item);
};

namespace curlpp {
class Easy;
}

class Html
{
private:
  //请求
  static curlpp::Easy request;
  ///提供一个cookie_list供操作
  static std::string cookie_list;
public:
  
  /**
   * 提取html的 \<a> 标签 的 href属性 <br>
   * @param html
   * @return 链接字符串数组
   */
  static std::vector<std::string> extractLinks(const std::string &html);
  
  /**
   * 提取html的 \<form> 标签的属性 method,action <br>
   * 以及表单内部的 \<input>,\<select> 标签的属性 name，value <br>
   * @param html
   * @return 提取出的表单数组
   */
  static std::vector<Form> extractForms(const std::string &html);
  
  /**
   * 通过表单访问网页，会使用上次请求的cookie
   * @param form 表单
   * @return 请求的网页
   */
  
  static std::string httpRequest(const Form &form);
  
  /**
   * 通过url访问网页
   * @param url 网页url
   * @return 响应的网页
   */
  static std::string httpRequest(const std::string &url);
  
};


#endif //WEBSCAN_HTML_H
