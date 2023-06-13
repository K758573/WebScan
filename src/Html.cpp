//
// Created by Shiro on 2023/6/13.
//

#include "Html.h"
#include "gumbo.h"

void searchForLinksHelper(GumboNode *node, std::vector <std::string> &url_lists)
{
  if (node->type != GUMBO_NODE_ELEMENT) {
    return;
  }
  GumboAttribute* href;
  if (node->v.element.tag == GUMBO_TAG_A &&
      (href = gumbo_get_attribute(&node->v.element.attributes, "href"))) {
    std::string temp(href->value);
    if (temp!="#") {
      url_lists.emplace_back(href->value);
    }
  }
  
  GumboVector* children = &node->v.element.children;
  for (unsigned int i = 0; i < children->length; ++i) {
    searchForLinksHelper(static_cast<GumboNode *>(children->data[i]),url_lists);
  }
}

std::vector<std::string> Html::searchForLinks(const std::string& raw_html)
{
  auto output = gumbo_parse(raw_html.c_str());
  std::vector<std::string> url_lists;
  searchForLinksHelper(output->root,url_lists);
  gumbo_destroy_output(&kGumboDefaultOptions, output);
  return url_lists;
}

