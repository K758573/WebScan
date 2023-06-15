//
// Created by Shiro on 2023/6/15.
//
#include "src/Utils.h"
#include "src/Html.h"
#include <iostream>
#include <QNetworkAccessManager>

int main()
{
  using namespace std;
  std::string url = "http://101.43.144.98/vul/xss/xsspost/post_login.php";
//    auto html = Utils::downloadUrlToString("http://101.43.144.98/vul/xss/xss_reflected_get.php");
    auto html = Utils::downloadUrlToString("http://101.43.144.98/vul/xss/xsspost/post_login.php");
//  auto html = Utils::downloadUrlToString("http://101.43.144.98/vul/sqli/sqli_id.php");
  auto ret = Html::extractForms(html);
  for (const auto &item: ret) {
    cout << item.method << "\n" << item.action << "\n";
    for (const auto &it: item.args) {
      cout << it.first << "=" << it.second << '\n';
    }
  }
  try{
    QNetworkAccessManager manager;
    QNetworkRequest request;
    request.setUrl(QString::fromStdString(url));
    request.setHeader(QNetworkRequest::ContentTypeHeader, QString::fromStdString(Form::CONTENT_TYPE));
    auto ret = manager.get(request);
    cout << ret;
  }catch (std::exception &ec){
    cout << ec.what();
  }
}