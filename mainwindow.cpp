//
// Created by Shiro on 2023/6/14.
//

// You may need to build the project (run Qt uic code generator) to get "ui_MainWindow.h" resolved

#include "mainwindow.h"
#include "ui_MainWindow.h"
#include <QStandardItem>


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent), ui(new Ui::MainWindow)
{
  ui->setupUi(this);
  form_window = new FormWindow(nullptr);
  connect(this, &MainWindow::sendFormData, form_window, &FormWindow::receiveFormData);
  connect(ui->btn_get_url_list, &QPushButton::clicked, this, &MainWindow::initUrlData);
  connect(ui->tree_url_list, &QTreeView::clicked, ui->browser_page, [this](const QModelIndex index) {
    QString url = index.data().toString();
    current_url = url;
//    if (!std::equal(url.begin(), url.begin() + 4, "http")) {
//      url = root_url + url;
//    }
    if (url_pages[url] == QString()) {
      url_pages[url] = QString::fromStdString(Utils::downloadUrlToString(url.toStdString()));
      auto temp_forms = Html::extractForms(url_pages[url].toStdString());
      //表单action纠正
      for (auto &it: temp_forms) {
        it.action = current_url.toStdString();
      }
      url_forms[url] = QVector<Form>(temp_forms.begin(), temp_forms.end());
    }
    ui->browser_page->setHtml(url_pages[url], root_url);
    ui->statusbar->showMessage(url);
  });
  connect(ui->act_extract_form, &QAction::triggered, this, [this] {
    form_window->show();
    emit sendFormData(url_forms[current_url]);
  });
}

MainWindow::~MainWindow()
{
  delete ui;
}

void MainWindow::initUrlData()
{
  auto model = new QStandardItemModel(this);
  ui->tree_url_list->setHeaderHidden(true);
  ui->tree_url_list->setModel(model);
  ui->tree_url_list->expandAll();
  root_url = ui->edit_line_url->text();
  url_pages.insert(root_url,
                   QString::fromStdString(Utils::downloadUrlToString(ui->edit_line_url->text().toStdString())));
  auto it = new QStandardItem(ui->edit_line_url->text());
  model->setItem(0, 0, it);
  //提取页面所有url
  auto temp = Html::extractLinks(url_pages[root_url].toStdString());
  //处理url转为可直接访问的绝对路径
  QList<QStandardItem *> temp_list;
  for (const auto &item: temp) {
    QString url = QString::fromStdString(item);
    if (!std::equal(url.begin(), url.begin() + 4, "http")) {
      url = root_url + url;
    }
    url_pages.insert(url, QString());
    temp_list.append(new QStandardItem(url));
  }
  for (int i = 0; i < temp_list.size(); ++i) {
    model->item(0, 0)->setChild(i, temp_list[i]);
  }
  
}
