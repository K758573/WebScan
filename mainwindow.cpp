//
// Created by Shiro on 2023/6/14.
//

// You may need to build the project (run Qt uic code generator) to get "ui_MainWindow.h" resolved

#include "mainwindow.h"
#include "ui_MainWindow.h"
#include <QStandardItem>
#include <QWebEngineProfile>
#include <QWebEngineCookieStore>
#include "src/Log.h"

const QString HTTP_LINK_PREFIX = "http://";
const QString HTTPS_LINK_PREFIX = "https://";

void correctFormAction(const QString &url, QVector<Form> &forms);

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent), ui(new Ui::MainWindow)
{
  ui->setupUi(this);
  form_window = new FormWindow(nullptr);
  //输入链接，点击抓取，提取网页的url，保存到url_page中，url_page中保存的是完整的地址
  connect(ui->btn_get_url_list, &QPushButton::clicked, this, &MainWindow::onButtonGetUrlListClicked);
  //点击节点中的链接，返回该节点的页面
  connect(ui->tree_url_list, &QTreeView::clicked, this, &MainWindow::onTreeUrlListClicked);
  //点击提取表单，弹出表单窗口
  connect(ui->act_extract_form, &QAction::triggered, this, &MainWindow::onActionExtractForm);
  //建立窗口之间的通信，用于传输表单
  connect(this, &MainWindow::sendFormData, form_window, &FormWindow::receiveFormData);
  connect(ui->browser_page->page()->profile()->cookieStore(),
          &QWebEngineCookieStore::cookieAdded,
          this,
          [&](const QNetworkCookie& cookie) {
            LOG("{}",cookie.name().toStdString());
            LOG("{}",cookie.value().toStdString());
          });
}

MainWindow::~MainWindow()
{
  delete ui;
}

void MainWindow::onButtonGetUrlListClicked()
{
  auto model = new QStandardItemModel(this);
  ui->tree_url_list->setHeaderHidden(true);
  ui->tree_url_list->setModel(model);
  //读url，添加/
  root_url = ui->edit_line_url->text();
  if (!root_url.endsWith("/")) {
    root_url.push_back('/');
  }
  auto response = Html::httpRequest(ui->edit_line_url->text().toStdString());
  url_pages.insert(root_url, QString::fromStdString(response));
  auto it = new QStandardItem(ui->edit_line_url->text());
  model->setItem(0, 0, it);
  //提取页面所有url
  auto temp = Html::extractLinks(url_pages[root_url].toStdString());
  //处理url转为可直接访问的绝对路径
  QList<QStandardItem *> temp_list;
  for (const auto &item: temp) {
    QString url = QString::fromStdString(item);
    if (url.startsWith(HTTP_LINK_PREFIX, Qt::CaseInsensitive) ||
        url.startsWith(HTTPS_LINK_PREFIX, Qt::CaseInsensitive)) {
    } else {
      url = root_url + url;
    }
    
    url_pages.insert(url, QString());
    temp_list.append(new QStandardItem(url));
  }
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
    url_pages[url] = QString::fromStdString(Html::httpRequest(url.toStdString()));
  }
  ui->browser_page->setHtml(url_pages[url], root_url);
  ui->statusbar->showMessage(url);
}

void MainWindow::onActionExtractForm()
{
  //表单界面的url
  auto url = ui->tree_url_list->currentIndex().data().toString();
  //查找缓存，提取表单，添加到缓存变量
  if (url_forms.find(url) == url_forms.end()) {
    auto forms = Html::extractForms(url_pages[url].toStdString());
    //修正表单action的url
    url_forms[url] = QVector<Form>(forms.begin(), forms.end());
    correctFormAction(url, url_forms[url]);
  }
  
  emit sendFormData(url_forms[url]);
}

///表单地址转绝对路径
void correctFormAction(const QString &url, QVector<Form> &forms)
{
  for (auto &form: forms) {
    const auto &action = form.action;
    if (url.endsWith(QString::fromStdString(action)) || action.empty() || action.starts_with('#') ||
        action.starts_with(' ')) {
      //当前路径
      form.action = url.toStdString();
      continue;
    } else if (action.starts_with("http://") || action.starts_with("https://")) {
      //绝对路径
      continue;
    }
    //相对路径
    //提取最后一个/后的文件名
    std::string url_x = url.toStdString();
    auto pos = url_x.rfind('/');
    std::string filename;
    if (pos < form.action.size()) {
      url_x.erase(pos + 1);
    }
    form.action = url_x + form.action;
  }
}