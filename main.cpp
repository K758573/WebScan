#include <iostream>
#include <urlmon.h>
#include <string>
#include <fstream>
#include <vector>
#include "src/Utils.h"
#include "src/Html.h"
const std::string TEMP_FILE_NAME("temp.txt");
const std::string MY_TEST_WEB("http://www.758573.shop");

int main()
{
  using namespace std;
  auto raw_html = Utils::downloadUrlToString(MY_TEST_WEB);
  auto url_lists = Html::searchForLinks(raw_html);
  for (const auto &item: url_lists) {
    cout << item << '\n';
  }
  return 0;
}
