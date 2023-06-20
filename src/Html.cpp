//
// Created by Shiro on 2023/6/13.
//

#include <cassert>
#include <curlpp/Easy.hpp>
#include <curlpp/cURLpp.hpp>
#include <curlpp/Options.hpp>
#include <curlpp/Infos.hpp>
#include <sstream>
#include "Html.h"
#include "gumbo.h"
#include "Log.h"

const std::string Form::CONTENT_TYPE = "application/x-www-form-urlencoded";

std::ostream &operator<<(std::ostream &os, const Form &form)
{
  os << "method: " << form.method << " action: " << form.action << "\nargs:\n";
  for (const auto &item: form.args) {
    os << item.first << "=" << item.second << '\n';
  }
  return os;
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

std::vector<std::string> Html::extractLinks(const std::string &html)
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
  if (node->v.element.tag == GUMBO_TAG_INPUT || node->v.element.tag == GUMBO_TAG_SELECT) {
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

std::vector<Form> Html::extractForms(const std::string &html)
{
  auto output = gumbo_parse(html.c_str());
  std::vector<Form> forms;
  extractFormNodes(output->root, forms);
  return forms;
}

std::string Html::httpRequest(const Form &form)
{
  std::string url = form.action;
  std::stringstream answer;
  try {
    std::string data;
    curlpp::Cleanup cleanup;
    curlpp::Easy request;
    for (const auto &it: form.args) {
      data.append(curlpp::escape(it.first)).append("=").append(curlpp::escape(it.second)).push_back('&');
    }
    if (data.ends_with('&')) {
      data.erase(data.end() - 1, data.end());
    }
    if (form.method == "get") {
      url.append("?").append(data);
    } else if (form.method == "post") {
      request.setOpt(curlpp::options::PostFields(data));
    }
    request.setOpt(new curlpp::options::Url(url));
    request.setOpt(curlpp::options::WriteStream(&answer));
    request.perform();
  } catch (std::exception &e) {
    LOG("{}", e.what());
  }
  return answer.str();
}

std::string Html::httpRequest(const std::string &url)
{
  std::stringstream answer;
  try {
    curlpp::Cleanup cleanup;
    curlpp::Easy request;
    request.setOpt(new curlpp::options::Url(url));
    request.setOpt(curlpp::options::WriteStream(&answer));
    request.perform();
    
  } catch (std::exception &e) {
    LOG("{}", e.what());
  }
  return answer.str();
}

std::string Html::httpRequestGetCookie(const std::string &url, std::list<std::string> *cookie)
{
  std::stringstream answer;
  try {
    curlpp::Cleanup cleanup;
    curlpp::Easy request;
    request.setOpt(new curlpp::options::Url(url));
    request.setOpt(new curlpp::options::WriteStream(&answer));
    request.setOpt(new curlpp::options::CookieList(""));
    request.perform();
    curlpp::infos::CookieList::get(request, *cookie);
  } catch (std::exception &e) {
    LOG("{}", e.what());
  }
  return answer.str();
}

std::string Html::httpRequestSetCookie(const std::string &url, std::list<std::string> *cookie)
{
  std::stringstream answer;
  try {
    curlpp::Cleanup cleanup;
    curlpp::Easy request;
    request.setOpt(new curlpp::options::Url(url));
    request.setOpt(new curlpp::options::WriteStream(&answer));
    for (const auto &it: *cookie) {
      request.setOpt(new curlpp::options::CookieList(it));
    }
    request.perform();
  } catch (std::exception &e) {
    LOG("{}", e.what());
  }
  return answer.str();
}

std::string Html::httpRequestWithCookie(const Form &form, const std::list<std::string> *cookie)
{
  std::string url = form.action;
  std::stringstream answer;
  try {
    std::string data;
    curlpp::Cleanup cleanup;
    curlpp::Easy request;
    for (const auto &it: form.args) {
      data.append(curlpp::escape(it.first)).append("=").append(curlpp::escape(it.second)).push_back('&');
    }
    if (data.ends_with('&')) {
      data.erase(data.end() - 1, data.end());
    }
    if (form.method == "get") {
      url.append("?").append(data);
    } else if (form.method == "post") {
      request.setOpt(curlpp::options::PostFields(data));
    }
    for (const auto &it: *cookie) {
      request.setOpt(new curlpp::options::CookieList(it));
    }
    request.setOpt(new curlpp::options::Url(url));
    request.setOpt(curlpp::options::WriteStream(&answer));
    request.perform();
  } catch (std::exception &e) {
    LOG("{}", e.what());
  }
  return answer.str();
}

