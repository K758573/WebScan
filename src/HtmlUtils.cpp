//
// Created by Shiro on 2023/6/23.
//

#include <iostream>
#include <cassert>
#include "Log.h"
#include "HtmlUtils.h"
#include "gumbo.h"
#include "HttpRequest.h"

namespace WebScan {


CookieItem CookieItem::fromstring(const std::string &cookie_str)
{
  auto c = std::count(cookie_str.begin(), cookie_str.end(), '\t');
  CookieItem cookie_item;
  if (c != 6) {
    LOG("{}", "cookie list item字符串解析失败");
    return cookie_item;
  }
  size_t first = 0, last;
  last = cookie_str.find('\t', first);
  cookie_item.domain_ = cookie_str.substr(first, last - first);
  first = last + 1;
  last = cookie_str.find('\t', first);
  cookie_item.tail_ = cookie_str.substr(first, last - first);
  first = last + 1;
  last = cookie_str.find('\t', first);
  cookie_item.path_ = cookie_str.substr(first, last - first);
  first = last + 1;
  last = cookie_str.find('\t', first);
  cookie_item.secure_ = cookie_str.substr(first, last - first);
  first = last + 1;
  last = cookie_str.find('\t', first);
  cookie_item.expires_ = cookie_str.substr(first, last - first);
  first = last + 1;
  last = cookie_str.find('\t', first);
  cookie_item.name_ = cookie_str.substr(first, last - first);
  first = last + 1;
  last = cookie_str.find('\t', first);
  cookie_item.value_ = cookie_str.substr(first, last - first);
  return cookie_item;
}

std::ostream &operator<<(std::ostream &os, const CookieItem &item)
{
  os << "domain_: " << item.domain_ << "tail_: " << item.tail_ << " path_: " << item.path_ << " secure_: "
     << item.secure_ << " expires_: " << item.expires_ << " name: " << item.name_ << " value: " << item.value_;
  return os;
}

std::string CookieItem::toCookieStr() const
{
  return domain_ + '\t' + tail_ + '\t' + path_ + '\t' + secure_ + '\t' + expires_ + '\t' + name_ + '\t' + value_;
}

const std::string &CookieItem::name() const
{
  return name_;
}

void CookieItem::name(const std::string &name)
{
  name_ = name;
}

const std::string &CookieItem::value() const
{
  return value_;
}

void CookieItem::value(const std::string &value)
{
  value_ = value;
}

void searchForLinksHelper(GumboNode *node, std::vector<std::string> &url_lists)
{
  if (node->type != GUMBO_NODE_ELEMENT) {
    return;
  }
  GumboAttribute *href;
  if (node->v.element.tag == GUMBO_TAG_A) {
    href = gumbo_get_attribute(&node->v.element.attributes, "href");
    if (href != nullptr) {
      std::string temp(href->value);
      if (temp != "#") {
        url_lists.emplace_back(href->value);
      }
    }
  }
  GumboVector *children = &node->v.element.children;
  for (unsigned int i = 0; i < children->length; ++i) {
    searchForLinksHelper(static_cast<GumboNode *>(children->data[i]), url_lists);
  }
}

std::vector<std::string> HtmlUtils::extractLinks(const std::string &html)
{
  //转为DOM树
  auto output = gumbo_parse(html.c_str());
  std::vector<std::string> url_lists;
  searchForLinksHelper(output->root, url_lists);
  gumbo_destroy_output(&kGumboDefaultOptions, output);
  return url_lists;
}

void extractFormInputHelper(GumboNode *node, std::vector<GumboNode *> &inputs)
{
  if (node->type != GUMBO_NODE_ELEMENT) {
    return;
  }
  if (node->v.element.tag == GUMBO_TAG_INPUT || node->v.element.tag == GUMBO_TAG_SELECT ||
      node->v.element.tag == GUMBO_TAG_TEXTAREA) {
    inputs.push_back(node);
    return;
  }
  auto sub_nodes = &node->v.element.children;
  for (int i = 0; i < sub_nodes->length; ++i) {
    auto sub_node = (GumboNode *) sub_nodes->data[i];
    extractFormInputHelper(sub_node, inputs);
  }
}

/**
 * 提取节点中的 \<input> 标签
 * @param node
 * @return
 */
std::vector<GumboNode *> extractFormInput(GumboNode *node)
{
  std::vector<GumboNode *> inputs;
  extractFormInputHelper(node, inputs);
  return inputs;
}

void extractFormArgs(GumboNode *form_node, std::vector<Form> &forms)
{
  Form form;
  assert(form_node->type == GUMBO_NODE_ELEMENT);
  //提取参数，填充表单
  auto method = gumbo_get_attribute(&form_node->v.element.attributes, "method");
  auto action = gumbo_get_attribute(&form_node->v.element.attributes, "action");
  form.action = action == nullptr ? "" : action->value;
  form.method = method == nullptr ? "" : method->value;
  //遍历子节点，找input标签
  auto root_children = &form_node->v.document.children;
  for (int i = 0; i < root_children->length; ++i) {
    auto childs = extractFormInput((GumboNode *) root_children->data[i]);
    for (const auto &child: childs) {
      auto name = gumbo_get_attribute(&child->v.element.attributes, "name");
      auto value = gumbo_get_attribute(&child->v.element.attributes, "value");
      if (name != nullptr) {
        form.args[name->value] = "";
        if (value != nullptr) {
          form.args[name->value] = value->value;
        }
      }
    }
  }
  forms.push_back(form);
}


void extractFormNodes(GumboNode *node, std::vector<Form> &forms)
{
  //查找form标签
  if (node->type != GUMBO_NODE_ELEMENT)
    return;
  if (node->v.element.tag == GUMBO_TAG_FORM) {
    //提取form标签
    extractFormArgs(node, forms);
    //form的子标签不能是form
    return;
  }
  //子节点查找form标签
  GumboVector *children = &node->v.element.children;
  for (unsigned int i = 0; i < children->length; ++i) {
    extractFormNodes(static_cast<GumboNode *>(children->data[i]), forms);
  }
}

std::vector<Form> HtmlUtils::extractForms(const std::string &html)
{
  auto output = gumbo_parse(html.c_str());
  std::vector<Form> forms;
  extractFormNodes(output->root, forms);
  return forms;
}

std::string HtmlUtils::extractToken(const std::string &html)
{
  auto forms = extractForms(html);
  if (forms.empty()) {
    return "";
  }
  auto it = forms.front().args.find("token");
  if (it == forms.front().args.end()) {
    return "";
  }
  return it->second;
}
} // WebScan