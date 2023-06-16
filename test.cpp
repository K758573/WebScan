//
// Created by Shiro on 2023/6/15.
//
#include "src/Utils.h"
#include "src/Html.h"
#include <iostream>
#include <sstream>

int main()
{
  using namespace std;
  Form form{
    .method =  "get",
    .action =   "http://101.43.144.98/vul/xss/xsspost/post_login.php",
    .args = {{"name", "val"},{"a","b"}}
  };
  auto ret = Html::httpRequest(form);
  cout << ret;
}