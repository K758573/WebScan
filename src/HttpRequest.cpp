//
// Created by Shiro on 2023/6/23.
//

#include <curlpp/cURLpp.hpp>
#include <curlpp/Options.hpp>
#include <curlpp/Infos.hpp>
#include <ranges>
#include <string_view>

#include "HttpRequest.h"
#include "Log.h"

namespace WebScan {
//
// Created by Shiro on 2023/6/13.
//

const std::string Form::CONTENT_TYPE = "application/x-www-form-urlencoded";

std::ostream &operator<<(std::ostream &os, const Form &form)
{
  os << "method: " << form.method << " action: " << form.action << " args:\n";
  for (const auto &item: form.args) {
    os << item.first << "=" << item.second << '\n';
  }
  return os;
}

std::string Form::encodeArgs() const
{
  std::string ret;
  if (args.empty()) {
    return ret;
  }
  for (const auto &item: args) {
    ret.append(curlpp::escape(item.first));
    ret.push_back('=');
    ret.append(curlpp::escape(item.second));
    ret.push_back('&');
  }
  if (ret.back() == '&') {
    ret.pop_back();
  }
  return ret;
}

std::string Form::tostring() const
{
  std::stringstream ssm;
  ssm << *this;
  return ssm.str();
}

std::string HttpRequest::request(const Form &form)
{
  std::string url = form.action;
  std::string args = form.encodeArgs();
  std::string cookie;
  response_.str("");
  try {
    //请求方式
    curlpp::Easy easy;
    //回显流
    easy.setOpt(new curlpp::options::WriteStream(&response_));
    if (form.method == "get") {
      url.append("?").append(args);
    } else if (form.method == "post") {
      easy.setOpt(new curlpp::options::PostFields(args));
    }
    //url
    easy.setOpt(new curlpp::options::Url(url));
    //cookie
    for (const auto &item: cookie_list_) {
      cookie.append(item).push_back('\n');
    }
    easy.setOpt(new curlpp::options::CookieList(cookie));
    //开始请求
    easy.perform();
    //更新服务器返回的cookie
    curlpp::infos::CookieList::get(easy, cookie_list_);
  } catch (std::exception &e) {
    LOG("{}", e.what());
  }
  return response_.str();
}

std::string HttpRequest::request(const std::string &url)
{
  Form form;
  form.method = "get";
  form.action = url;
  return request(form);
}


std::list<std::string> &HttpRequest::cookielist()
{
  return cookie_list_;
}

void HttpRequest::cookielist(const std::list<std::string> &cookielist)
{
  cookie_list_ = cookielist;
}

void HttpRequest::reset()
{
  cookie_list_.clear();
}

} // WebScan