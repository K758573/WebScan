//
// Created by Shiro on 2023/6/13.
//

#ifndef WEBSCAN_HTML_H
#define WEBSCAN_HTML_H

#include <vector>
#include <string>
#include <unordered_map>
#include <ostream>

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


class Html
{
private:
//  curlpp::Easy easy;
public:
  /**
   * 提取html的 \<a> 标签 的 href属性 <br>
   * @param html
   * @return 链接字符串数组
   */
  static std::vector<std::string> extractLinks(const std::string& html);
  
  /**
   * 提取html的 \<form> 标签的属性 method,action <br>
   * 以及表单内部的 \<input>,\<select> 标签的属性 name，value <br>
   * @param html
   * @return 提取出的表单数组
   */
  static std::vector<Form> extractForms(const std::string& html);
  
  /**
   * 通过表单访问网页
   * @param form 表单
   * @return 请求的网页
   */
  
  static std::string httpRequest(const Form &form);
  /**
   * 携带cookie的表单请求
   * @param form
   * @param cookie
   * @return
   */
  static std::string httpRequestWithCookie(const Form &form, const std::list<std::string> *cookie);
  
  /**
   * 通过url访问网页
   * @param url 网页url
   * @return 响应的网页
   */
  static std::string httpRequest(const std::string &url);
  /**
   * http请求，请求完成后 获取Cookie
   * @param url
   * @param cookie 返回参数
   * @return
   */
  static std::string httpRequestGetCookie(const std::string &url, std::list<std::string> *cookie);
  /**
   * 携带cookie进行http请求
   * @param url
   * @param cookie
   * @return
   */
  static std::string httpRequestSetCookie(const std::string &url, std::list<std::string> *cookie);
};


#endif //WEBSCAN_HTML_H
