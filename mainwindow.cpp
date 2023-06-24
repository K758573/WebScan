//
// Created by Shiro on 2023/6/14.
//

// You may need to build the project (run Qt uic code generator) to get "ui_MainWindow.h" resolved

#include "mainwindow.h"
#include "ui_MainWindow.h"
#include <QStandardItem>
#include <QWebEngineProfile>
#include <QWebEngineCookieStore>
#include <algorithm>

const auto HTTP_LINK_PREFIX = "http://";
const auto HTTPS_LINK_PREFIX = "https://";

/**
 * 根据当前url和表单action的url 修改表单action为绝对路径
 * @param url 当前页面url
 * @param forms
 */
void correctFormAction(const QString &url, QVector<Form> &forms);

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent), ui(new Ui::MainWindow),form_window(nullptr)
{
  ui->setupUi(this);
  //输入链接，点击抓取，提取网页的url，保存到url_page中，url_page中保存的是完整的地址
  connect(ui->btn_get_url_list, &QPushButton::clicked, this, &MainWindow::onButtonGetUrlListClicked);
  //点击节点中的链接，返回该节点的页面
  connect(ui->tree_url_list, &QTreeView::clicked, this, &MainWindow::onTreeUrlListClicked);
  //点击提取表单，弹出表单窗口
  connect(ui->act_extract_form, &QAction::triggered, this, &MainWindow::onActionExtractForm);
  connect(ui->browser_page->page()->profile()->cookieStore(),
          &QWebEngineCookieStore::cookieAdded,
          this,
          [&](const QNetworkCookie &cookie) {
            for (auto &item: request.cookielist()) {
              CookieItem ci = CookieItem::fromstring(item);
              if (ci.name() == cookie.name().toStdString()) {
                LOG("curl的cookie更新为浏览器cookie,name = {} , value: {} -->> {}",
                    ci.name(),
                    ci.value(),
                    cookie.value().toStdString());
                ci.value(cookie.value().toStdString());
                item = ci.toCookieStr();
              }
            }
          });
}

MainWindow::~MainWindow()
{
  delete ui;
}

void MainWindow::onButtonGetUrlListClicked()
{
  //url树形结构
  auto model = new QStandardItemModel(this);
  ui->tree_url_list->setHeaderHidden(true);
  ui->tree_url_list->setModel(model);
  //读url，添加/
  root_url = ui->edit_line_url->text();
  if (root_url.back() != '/') {
    root_url.push_back('/');
  }
  auto response = request(root_url.toStdString());
  url_pages.insert(root_url, QString::fromStdString(response));
  auto it = new QStandardItem(ui->edit_line_url->text());
  model->setItem(0, 0, it);
  //提取页面所有url
  auto temp = HtmlUtils::extractLinks(url_pages[root_url].toStdString());
  QList<QStandardItem *> temp_list;
  for (const auto &item: temp) {
    QString url = QString::fromStdString(item);
    //处理url转为可直接访问的绝对路径
    if (url.startsWith(HTTP_LINK_PREFIX, Qt::CaseInsensitive) ||
        url.startsWith(HTTPS_LINK_PREFIX, Qt::CaseInsensitive)) {
    } else {
      url = root_url + url;
    }
    url_pages[url] = "";
    temp_list.append(new QStandardItem(url));
  }
  //为树添加子项
  for (int i = 0; i < temp_list.size(); ++i) {
    model->item(0, 0)->setChild(i, temp_list[i]);
  }
  ui->tree_url_list->expandAll();
}

//点击url，更新页面
void MainWindow::onTreeUrlListClicked(QModelIndex index)
{
  //获取url
  QString url = index.data().toString();
  //查找页面，如果有缓存，直接取出
  if (url_pages[url] == QString()) {
    url_pages[url] = QString::fromStdString(request(url.toStdString()));
  }
  ui->browser_page->setHtml(url_pages[url], root_url);
  ui->statusbar->showMessage(url);
}

void MainWindow::onActionExtractForm()
{
  if (form_window != nullptr) {
    delete form_window;
    form_window = nullptr;
  }
  form_window = new FormWindow(nullptr, request);
  //建立窗口之间的通信，用于传输表单
  connect(this, &MainWindow::sendFormData, form_window, &FormWindow::receiveFormData);
  //表单界面的url
  auto url = ui->tree_url_list->currentIndex().data().toString();
  //查找缓存，提取表单，添加到缓存变量
  if (url_forms.find(url) == url_forms.end()) {
    auto forms = HtmlUtils::extractForms(url_pages[url].toStdString());
    //修正表单action的url
    url_forms[url] = QVector<Form>(forms.begin(), forms.end());
    correctFormAction(url, url_forms[url]);
  }
  emit sendFormData(url_forms[url]);
}

void correctFormAction(const QString &url, QVector<Form> &forms)
{
  for (auto &form: forms) {
    const auto &action = form.action;
    if (url.endsWith(QString::fromStdString(action)) || action.empty() || action.starts_with('#')) {
      //使用当前路径
      form.action = url.toStdString();
      continue;
    } else if (action.starts_with(HTTPS_LINK_PREFIX) || action.starts_with(HTTPS_LINK_PREFIX)) {
      //绝对路径
      continue;
    }
    //相对路径
    //提取最后一个/后的文件名
    std::string url_x = url.toStdString();
    auto pos = url_x.rfind('/');
    if (pos < form.action.size()) {
      url_x.erase(pos + 1);
    }
    form.action = url_x + form.action;
  }
}