//
// Created by Shiro on 2023/6/13.
//

#ifndef WEBSCAN_HTML_H
#define WEBSCAN_HTML_H

#include <vector>
#include <string>
#include <unordered_map>

/**
 * 表单，只包括可提交部分
 */
class Form
{
public:
  static const std::string CONTENT_TYPE;
  std::string method;
  std::string action;
  std::unordered_map<std::string, std::string> args;
  
};


class Html
{
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
  
};


#endif //WEBSCAN_HTML_H
