//
// Created by Shiro on 2023/6/23.
//

#ifndef WEBSCAN_HTMLUTILS_H
#define WEBSCAN_HTMLUTILS_H
#include <string>
#include <vector>

namespace WebScan {

class Form;
/**
 * 工具类，用于操作html界面，提取界面元素等
 */
class HtmlUtils
{
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
   * 提取页面中的token值
   * @param html
   * @return
   */
  static std::string extractToken(const std::string &html);
};

/**
 * 辅助类
 * 用于操作netscape表示的cookie字符串
 */
class CookieItem
{
public:
  std::string domain_;
  std::string tail_ = "FALSE";
  std::string path_ = "/";
  std::string secure_ = "FALSE";
  std::string expires_ = "0";
  std::string name_;
  std::string value_;
public:
  /**
   * 把netscape格式的cookie字符串转为 CookieItem 结构
   * @param cookie_str
   * @return CookieItem
   */
  static CookieItem fromstring(const std::string &cookie_str);
  
  friend std::ostream &operator<<(std::ostream &os, const CookieItem &item);
  /**
   * 把CookieItem结构转为netscape格式的cookie字符串
   * @return
   */
  [[nodiscard]] std::string toCookieStr() const;
  
  const std::string &name() const;
  
  void name(const std::string &name);
  
  const std::string &value() const;
  
  void value(const std::string &value);
};
} // WebScan

#endif //WEBSCAN_HTMLUTILS_H
