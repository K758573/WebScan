//
// Created by Shiro on 2023/6/15.
//
#include "src/Utils.h"
#include "src/Html.h"
#include <iostream>
#include <sstream>
#include <curlpp/cURLpp.hpp>
#include <curlpp/Easy.hpp>
#include <curlpp/Options.hpp>
#include <curlpp/Exception.hpp>

static std::string httpSubmit(std::string url, const Form &form)
{
  url = form.action;
  std::stringstream answer;
  std::string data;
  curlpp::Cleanup cleanup;
  curlpp::Easy request;
  for (const auto &it: form.args) {
    data.append(it.first).append("=").append(it.second).push_back('&');
  }
  if (data.ends_with('&')) {
    data.erase(data.end()-1,data.end());
  }
  if (form.method == "get") {
    url.append("?").append(data);
  } else if (form.method == "post") {
    request.setOpt(curlpp::options::PostFields(data));
  }
  request.setOpt(new curlpp::options::Url(url));
  request.setOpt(curlpp::options::WriteStream(&answer));
  request.perform();
  return answer.str();
}

int main()
{
  using namespace std;
  Form form{
    .method =  "get",
    .action =   "http://101.43.144.98/vul/xss/xsspost/post_login.php",
    .args = {{"name", "val"},{"a","b"}}
  };
  auto ret = httpSubmit("", form);
  cout << ret;
}