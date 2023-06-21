/**
* \file
* Cookies.
*
*/

#include <string>
#include <sstream>
#include <utility>
#include <vector>
#include <iostream>
#include <cstdlib>
#include <ctime>
#include <curlpp/cURLpp.hpp>
#include <curlpp/Easy.hpp>
#include <curlpp/Infos.hpp>
#include <curlpp/Options.hpp>
#include <curlpp/Types.hpp>
#include "src/Log.h"
#include "src/Html.h"
#include <fstream>
#include <array>


int main()
{
  std::list<std::string> cookieList;
  std::string cookie = "101.43.144.98\tFALSE\t/\tFALSE\t0\tPHPSESSID\t5ig0q4etjl6eba80503fqkshn4\n";
  std::string cookie2 = "101.43.144.98\tFALSE\t/\tFALSE\t0\tPHPSssESSID\t5ig0q4etjl6eba803aafqkshn4\n";
  try {
    curlpp::Easy easy;
    CookieItem ci;
    ci.domain = ".baidu.com";
    ci.tail = "TRUE";
    ci.path = "/";
    ci.secure = "FALSE";
    ci.expires = "3834811896";
    ci.name = "BAIDUID";
    ci.value = "98777DF99142266ADD0551FD8E2452DC:FG=1";
    easy.setOpt(new curlpp::options::Url("https://www.baidu.com/"));
    easy.setOpt(new curlpp::options::WriteStream(new std::ofstream("a.txt")));
//    easy.setOpt(new curlpp::options::PostFields ("username=a&password=b&vcode=r4iqkj"));
    easy.setOpt(new curlpp::options::CookieList(ci.toCookieListItem()+"www.baidu.com   FALSE   /       FALSE   1687328749      BD_NOT_HTTPS    1\n"+".baidu.com      TRUE    /       FALSE   3834812096      PSTM    1687328451\n"));
    easy.perform();
    curlpp::infos::CookieList::get(easy, cookieList);
    for (const auto &item: cookieList)
      std::cout << item << '\n';
  } catch (std::exception &e) {
  
  }
  return 0;
}