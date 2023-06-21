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
public:
  std::string domain;
  std::string tail = "FALSE";
  std::string path = "/";
  std::string secure = "FALSE";
  std::string expires = "0";
  std::string name;
  std::string value;
private:
  std::string cookie_list_item_;
public:
  static CookieItem fromstring(const std::string &cookie_list_item);
  
  friend std::ostream &operator<<(std::ostream &os, const CookieItem &item);
  
  [[nodiscard]] std::string toCookieListItem() const;
};

namespace curlpp {
class Easy;
}

class Html
{
public:
//  static std::vector<CookieItem> cookie_list;
  static std::list<std::string> cookie_list;

private:
  ///提供一个cookie_list供操作
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
   * @param status 返回参数 请求和响应使用相同cookie，返回true
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
