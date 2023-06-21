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
  curlpp::Easy easy;
  easy.setOpt(new curlpp::options::Url("http://101.43.144.98/vul/burteforce/bf_server.php"));
  easy.setOpt(new curlpp::options::WriteStream(new std::ofstream("a.txt")));
  easy.setOpt(new curlpp::options::CookieList(""));
  std::vector<std::string> cookieList;
  return 0;
}