//
// Created by Shiro on 2023/6/13.
//

#ifndef WEBSCAN_HTML_H
#define WEBSCAN_HTML_H

#include <vector>
#include <string>

class Html
{
public:
  static std::vector<std::string> searchForLinks(const std::string& raw_html);

};


#endif //WEBSCAN_HTML_H
