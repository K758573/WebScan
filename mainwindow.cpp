//
// Created by Shiro on 2023/6/14.
//

// You may need to build the project (run Qt uic code generator) to get "ui_MainWindow.h" resolved

#include "mainwindow.h"
#include "ui_MainWindow.h"
#include <QStandardItem>
#include "src/Utils.h"
#include "src/Html.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent), ui(new Ui::MainWindow)
{
  ui->setupUi(this);
  connect(ui->btn_get_url_list, &QPushButton::clicked, this, &MainWindow::initUrlData);
  connect(ui->tree_url_list, &QTreeView::clicked, ui->browser_page, [this](const QModelIndex index) {
    QString url = index.data().toString();
    if (!std::equal(url.begin(), url.begin() + 4, "http")) {
      url = root_url + url;
    }
    if (url_pages[url] == QString()) {
      url_pages[url] = QString::fromStdString(Utils::downloadUrlToString(url.toStdString()));
    }
    ui->browser_page->setHtml(url_pages[url], root_url);
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
  auto temp = Html::extractLinks(url_pages[root_url].toStdString());
  QList<QStandardItem *> temp_list;
  for (const auto &item: temp) {
    QString url = QString::fromStdString(item);
    url_pages.insert(url, QString());
    temp_list.append(new QStandardItem(url));
  }
  for (int i = 0; i < temp_list.size(); ++i) {
    model->item(0, 0)->setChild(i, temp_list[i]);
  }
  
}
