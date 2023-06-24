//
// Created by Shiro on 2023/6/23.
//

#ifndef WEBSCAN_HTTPREQUEST_H
#define WEBSCAN_HTTPREQUEST_H

#include <string>
#include <unordered_map>
#include <sstream>

namespace WebScan {
/**
 * Form表单，包含请求方式，action地址，请求参数
 */
class Form
{
public:
  //请求方式为POST时，请求头默认添加的字段
  static const std::string CONTENT_TYPE;
  //请求方式
  std::string method;
  //请求的绝对地址
  std::string action;
  //请求参数
  std::unordered_map<std::string, std::string> args;
  
  friend std::ostream &operator<<(std::ostream &os, const Form &form);

  std::string tostring() const;
  
public:
  /**
   * 对参数进行编码，使用&符号连接 <br>
   * (a,b),(c,d)  ----- >>>>  a=b&c=d
   * @return 编码后的字符串
   */
  [[nodiscard]] std::string encodeArgs() const;
};


class HttpRequest
{
private:
  /**
   * 保存上次请求的Cookie，每次请求都会更新这个cookie
   */
  std::list<std::string> cookie_list_;
  std::stringstream response_;
  
public:
  /**
   * 获取上次请求的cookie字符串
   * @return
   */
  std::list<std::string> &cookielist();
  
  /**
   * 设置下次请求的cookie字符串
   * @param cookielist
   */
  void cookielist(const std::list<std::string> &cookielist);

public:
  /**
   * 重置cookie
   */
  void reset();
  
  /**
   * 通过表单访问网页，会使用上次请求的cookie
   * @param form 表单
   * @param status 返回参数 请求和响应使用相同cookie，返回true
   * @return 请求的网页
   */
  
  std::string request(const Form &form);
  
  /**
   * 通过url访问网页
   * @param url 网页url
   * @return 响应的网页
   */
  std::string request(const std::string &url);
  
  /**
   * 重载方法
   * @param form
   * @return
   */
  std::string operator()(const Form &form)
  {
    return request(form);
  }
  
  std::string operator()(const std::string &url)
  {
    return request(url);
  }
};


} // WebScan



#endif //WEBSCAN_HTTPREQUEST_H
